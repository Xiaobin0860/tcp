#include "network.h"
#include <thread>

using namespace Base;

static const int MAX_PACKET_SIZE = 256 * 1024 * 1024;
static const int BUFF_CAPACITY = 1024;
static const int WARNING_SEND_BUF_COUNT = 100;
static const int WARNING_RECV_BUF_SIZE = 100 * BUFF_CAPACITY;

#define CHECK_ERROR(error) if(CheckError(error)) return StartError(error)

//-----------------------------------------------------------------------------
// Hive
Hive::Hive(uint32_t idx) : _worker(new Worker(_io_service)), _running(true), _id(idx)
{
}
Hive::~Hive()
{
}

void Hive::SyncRunForever()
{
    while (_running)
    {
        try {
            ErrorCode ec;
            _io_service.run(ec);
            if (ec) break;
        }
        catch (std::exception& e) {
        }
    }
}

void Hive::Stop()
{
    if (_running.exchange(false))
    {
        _io_service.post([this] {
            _worker.reset();
            _io_service.stop();
        });
    }
}

void Hive::Reset()
{
    if (!_running.exchange(true))
    {
        _io_service.reset();
        _worker.reset(new Worker(_io_service));
    }
}
//-----------------------------------------------------------------------------

HivePool::HivePool(int size) : _next_idx(0), _pool_size(size)
{
    if (_pool_size < 1) _pool_size = std::thread::hardware_concurrency();
    for (int i=0; i<_pool_size; ++i) _pool.push_back(HivePtr(new Hive(i)));
}
HivePool::~HivePool()
{
}

const HivePtr& HivePool::GetHive()
{
    ++_next_idx;
    _next_idx %= _pool_size;
    return _pool[_next_idx];
}
const HivePtr& HivePool::GetHive(uint32_t key)
{
    return _pool[key%_pool_size];
}
void HivePool::AsyncRun()
{
    for (int i = 0; i < _pool_size; ++i)
    {
        std::thread t(std::bind(&Hive::SyncRunForever, _pool[i]));
        t.detach();
    }
}
void HivePool::Stop()
{
    for (int i = 0; i < _pool_size; ++i)
    {
        _pool[i]->Stop();
    }
}

//-----------------------------------------------------------------------------
// Server
Server::Server(int pool_size)
: _hive(new Hive(0))
, _acceptor(_hive->GetService())
, _error_state(false)
, _connection_id(0)
, _pool(pool_size)
{
}

Server::~Server()
{
}

void Server::HandleAccept(const ErrorCode& error, const ConnectionPtr& connection)
{
    CHECK_ERROR(error);
    
    if (connection->GetSocket().is_open())
    {
        if (OnAccept(connection, connection->GetSocket().remote_endpoint().address().to_string()))
        {
            connection->callbacks = callbacks;
            connection->OnConnect();
            connection->StartRecv();
        }
        else
        {
        }
    }
    else
    {
    }
    
    StartAccept();
}

void Server::Stop()
{
    _hive->Post(std::bind(&Server::StartError, shared_from_this(), boost::asio::error::connection_reset));
}

void Server::StartError(const ErrorCode& error)
{
    if (!_error_state.exchange(true))
    {
        ErrorCode ec;
        _acceptor.cancel(ec);
        _acceptor.close(ec);
        _hive->Stop();
        _pool.Stop();
    }
}

void Server::SyncRun()
{
    _pool.AsyncRun();
    _hive->Post(std::bind(&Server::StartAccept, shared_from_this()));
    _hive->SyncRunForever();
}
void Server::StartAccept()
{
    ++_connection_id;
    ConnectionPtr connection(new Connection(_pool.GetHive(), _connection_id));
    
    _acceptor.async_accept(connection->GetSocket(), std::bind(&Server::HandleAccept, shared_from_this(), std::placeholders::_1, connection));
}

void Server::Listen(const std::string& host, const std::string& port)
{
    Resolver resolver(_hive->GetService());
    Query query(host, port);
    EndPoint endpoint = *resolver.resolve(query);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(Acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Connection
Connection::Connection(const HivePtr& hive, uint32_t cid)
: _hive(hive)
, _socket(hive->GetService())
, _error_state(false)
, _connected(true)
, _connection_id(cid)
, buf_size_(0)
, read_index_(0)
, readable_size_(0)
, write_index_(0)
, writable_size_(0)
, _max_send_queue_size(-1)
{
    buf_size_ = 10 * BUFF_CAPACITY;
    recv_buf_.resize(buf_size_);
    write_index_ = read_index_ = 0;
    readable_size_ = 0;
    writable_size_ = recv_buf_.size();
}
Connection::~Connection()
{
}

void Connection::StartSend()
{
    if (!_pending_sends.empty())
    {
        boost::asio::async_write(_socket, boost::asio::buffer(_pending_sends.front()), std::bind(&Connection::HandleSend, shared_from_this(), std::placeholders::_1, _pending_sends.begin()));
    }
}

void Connection::StartRecv()
{
    _socket.async_read_some(boost::asio::buffer(_recv_buffer), std::bind(&Connection::HandleRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::StartError(const ErrorCode& error)
{
    if (!_error_state.exchange(true))
    {
        ErrorCode ec;
        _socket.shutdown(Socket::shutdown_both, ec);
        _socket.close(ec);
        OnError(error);
    }
}

void Connection::HandleConnect(const ErrorCode& error)
{
    CHECK_ERROR(error);
    
    if (_socket.is_open())
    {
        OnConnect();
        StartRecv();
        StartSend();
    }
    else
    {
        StartError(error);
    }
}

void Connection::HandleSend(const ErrorCode& error, BufferQueue::iterator itr)
{
    CHECK_ERROR(error);
    
    OnSend(*itr);
    _pending_sends.erase(itr);
    
    StartSend();
}

void Connection::HandleRecv(const ErrorCode& error, int32_t actual_bytes)
{
    CHECK_ERROR(error);
    
    OnRecv(_recv_buffer, actual_bytes);
    StartRecv();
}

void Connection::DispatchSend(Buffer buffer)
{
    auto size = _pending_sends.size();
    if (size >= _max_send_queue_size)
    {
        return;
    }
    _pending_sends.push_back(buffer);
    if (0 == size)
    {
        StartSend();
    }
}

void Connection::Connect(const std::string & host, const std::string& port)
{
    _error_state = false;
    ErrorCode ec;
    Resolver resolver(_hive->GetService());
    Query query(host, port);
    _socket.async_connect(*resolver.resolve(query), std::bind(&Connection::HandleConnect, shared_from_this(), std::placeholders::_1));
}

void Connection::Disconnect()
{
    _hive->Post(std::bind(&Connection::StartError, shared_from_this(), boost::asio::error::connection_reset));
}

void Connection::Send(const Buffer& buffer)
{
    _hive->Post(std::bind(&Connection::DispatchSend, shared_from_this(), buffer));
}

void Connection::OnConnect()
{
    _connected = true;
    if (callbacks.OnConnected) callbacks.OnConnected(shared_from_this());
}
void Connection::OnSend(const Buffer& buffer)
{
    if (callbacks.OnWrite) callbacks.OnWrite(shared_from_this(), buffer.size());
}
void Connection::OnRecv(uint8_t* data, int32_t len)
{
    if (callbacks.OnRead)
    {
        uint8_t* buf = _recv_buffer;
        
        if (len > writable_size_)
        {
            memcpy(&recv_buf_[0], &recv_buf_[0] + read_index_, readable_size_);
            read_index_ = 0;
            write_index_ = readable_size_;
            writable_size_ = buf_size_ - write_index_;
            if (len > writable_size_)
            {
                auto add_size = BUFF_CAPACITY;
                while (len > writable_size_ + add_size)
                {
                    add_size += BUFF_CAPACITY;
                }
                buf_size_ += add_size;
                writable_size_ += add_size;
                recv_buf_.resize(buf_size_);
            }
        }
        memcpy(&recv_buf_[0] + write_index_, buf, len);
        readable_size_ += len;
        writable_size_ -= len;
        write_index_ += len;
        
        if (readable_size_ > WARNING_RECV_BUF_SIZE)
        {
            //todo(lxb):
        }
        
        auto proc_len = callbacks.OnRead(shared_from_this(), &recv_buf_[0] + read_index_, readable_size_);
        if (proc_len != 0)
        {
            readable_size_ -= proc_len;
            read_index_ += proc_len;
        }
    }
}
void Connection::OnError(const ErrorCode& error)
{
    _connected = false;
    if (callbacks.OnDisconnect) callbacks.OnDisconnect(shared_from_this());
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PacketProcessor
void PacketProcessor::EncodePacket(const uint8_t* data, uint32_t len, Buffer& buf)
{
    buf.resize(len + 4);
    auto net_len = (uint32_t)htonl(len);
    memcpy(buf.data(), (uint8_t*)&net_len, 4);
    memcpy(buf.data() + 4, data, len);
}

uint32_t PacketProcessor::DecodePacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len)
{
    uint32_t total_len = 0;
    
    while (len >= 4)
    {
        int32_t pak_len = ntohl(*(int32_t *)data);
        if (pak_len <= 0 || pak_len > MAX_PACKET_SIZE)
        {
            conn->Disconnect();
            break;
        }
        auto cur_len = pak_len + 4;
        if (cur_len > len) break;
        
        ProcessPacket(conn, data + 4, pak_len);
        
        data += cur_len;
        len -= cur_len;
        total_len += cur_len;
    }
    
    return total_len;
}
