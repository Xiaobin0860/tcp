#include "GeneralClient.h"
#include <thread>

using namespace Base;

GeneralClient::GeneralClient()
: _hive(new Hive(0))
, _connection(new Connection(_hive, 0))
, _running(true)
, _connected(false)
, _reconnect_timer(_hive->GetService())
, _signals(_hive->GetService())
{
#if defined(SIGPIPE)
    _signals.add(SIGPIPE);
#endif
    
    _signals.async_wait(std::bind(&GeneralClient::signalHanlder, this, std::placeholders::_1, std::placeholders::_2));
    
    _connection->callbacks = {
        std::bind(&GeneralClient::OnConnect, this, std::placeholders::_1),
        std::bind(&GeneralClient::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&GeneralClient::OnWrite, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&GeneralClient::OnDisconnect, this, std::placeholders::_1)
    };
}

GeneralClient::~GeneralClient()
{
    ErrorCode ec;
    _signals.cancel(ec);
}

void GeneralClient::Connect(const std::string& host, const std::string& port)
{
    _running = true;
    _host = host;
    _port = port;
    _connection->Connect(_host, _port);
}
void GeneralClient::Disconnect()
{
    _running = false;
    _connection->Disconnect();
}

void GeneralClient::ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len)
{
}

void GeneralClient::SendPacket(const uint8_t* data, uint32_t len)
{
    Buffer buf;
    EncodePacket(data, len, buf);
    _connection->Send(buf);
}

void GeneralClient::SyncRun()
{
    _hive->SyncRunForever();
}
void GeneralClient::AsyncRun()
{
    std::thread t([this] {
        _hive->SyncRunForever();
    });
    t.detach();
}

void GeneralClient::Stop()
{
    Disconnect();
    _hive->Stop();
}

void GeneralClient::OnConnect(const ConnectionPtr conn)
{
    _connected = true;
}
uint32_t GeneralClient::OnRead(const ConnectionPtr& conn, const uint8_t* data, uint32_t len)
{
    return DecodePacket(conn, data, len);
}

void GeneralClient::OnWrite(const ConnectionPtr& conn, uint32_t len)
{
}

void GeneralClient::OnDisconnect(const ConnectionPtr conn)
{
    _connected = false;
    if (_running)
    {
        // reconnect
        _reconnect_timer.expires_from_now(boost::posix_time::seconds(5));
        _reconnect_timer.async_wait([this](const ErrorCode& e) {
            if (!e)
            {
                Connect(_host, _port);
            }
        });
    }
}

void GeneralClient::signalHanlder(const ErrorCode&e, int sig)
{
    auto deal = [this](const ErrorCode& e, int sig) {
        if (e) return;
#if defined(SIGPIPE)
        if (SIGPIPE == sig)
        {
            return;
        }
#endif
        if (_signal_calls.find(sig) != _signal_calls.end())
        {
            _signal_calls[sig]();
        }
    };
    
    deal(e, sig);
    
    _signals.async_wait(std::bind(&GeneralClient::signalHanlder, this, std::placeholders::_1, std::placeholders::_2));
}
