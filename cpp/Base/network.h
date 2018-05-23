#pragma once

#include <cstdint>
#include <memory>
#include <atomic>
#include <list>
#include <string>
#include <vector>
#include <unordered_map>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

namespace Base
{

#define RECV_BUFF_SIZE 1024

    class Connection;
    class Hive;

    using Func = std::function<void()>;
    using Timer = boost::asio::deadline_timer;
    using ErrorCode = boost::system::error_code;
    using IoService = boost::asio::io_service;
    using EndPoint = boost::asio::ip::tcp::endpoint;
    using Address = boost::asio::ip::address;
    using Resolver = boost::asio::ip::tcp::resolver;
    using Query = boost::asio::ip::tcp::resolver::query;
    using Acceptor = boost::asio::ip::tcp::acceptor;
    using Socket = boost::asio::ip::tcp::socket;
    using Worker = boost::asio::io_service::work;
    using WorkerPtr = std::shared_ptr<Worker>;
    using SignalSet = boost::asio::signal_set;
    using ConnectionPtr = std::shared_ptr<Connection>;
    using HivePtr = std::shared_ptr<Hive>;
    using FuncOnAccept = std::function<bool(const ConnectionPtr&, const std::string&)>;
    using FuncOnConnect = std::function<void(const ConnectionPtr)>;
    using FuncOnRead = std::function<uint32_t(const ConnectionPtr&, const uint8_t*, uint32_t)>;
    using FuncOnWrite = std::function<void(const ConnectionPtr&, uint32_t)>;
    using FuncOnDisconnect = std::function<void(const ConnectionPtr)>;
    using Buffer = std::vector<uint8_t>;
    using BufferQueue = std::list<Buffer>;

    struct ConnectionCallBacks
    {
        FuncOnConnect       OnConnected;
        FuncOnRead          OnRead;
        FuncOnWrite         OnWrite;
        FuncOnDisconnect    OnDisconnect;
    };

    //-----------------------------------------------------------------------------
    /*!
     * @class Hive wrapper IoService
     */
    class Hive : public std::enable_shared_from_this<Hive>, private boost::noncopyable
    {
    public:
        Hive(uint32_t idx);
        ~Hive();

        IoService& GetService() { return _io_service; }

        bool IsStopped() { return !_running; }

        uint32_t GetId() { return _id; }

        void SyncRun() { _io_service.run(); }
        void SyncRun(ErrorCode& ec) { _io_service.run(ec); }
        void SyncRunForever();

        void Stop();

        //Stop => Reset => Run...
        void Reset();

        void Post(const Func& func) { _io_service.post(func); }

    private:
        IoService _io_service;
        WorkerPtr _worker;
        std::atomic<bool> _running;
        uint32_t _id;
    };
    //-----------------------------------------------------------------------------

    class HivePool
    {
    public:
        HivePool(int size);
        ~HivePool();

        const HivePtr& GetHive();
        const HivePtr& GetHive(uint32_t key);

        void AsyncRun();
        void Stop();
    private:
        int _next_idx;
        int _pool_size;
        std::vector<HivePtr> _pool;
    };

    //-----------------------------------------------------------------------------
    /*!
     * @class Connection
     */
    class Connection : public std::enable_shared_from_this<Connection>, private boost::noncopyable
    {
        friend class Server;
        friend class Hive;

    public:
        Connection(const HivePtr& hive, uint32_t cid);
        virtual ~Connection();

        uint32_t GetId() { return _connection_id; }
        bool IsConnected() { return _connected; }

        const HivePtr& GetHive() { return _hive; }

        Socket& GetSocket() { return _socket; }

        uint32_t GetMaxSendQueueSize() { return _max_send_queue_size; }
        void SetMaxQueueSize(uint32_t size) { _max_send_queue_size = size; }

        void Connect(const std::string& host, const std::string& port);

        void Send(const Buffer& buffer);

        void Disconnect();

        ConnectionCallBacks callbacks;

    private:
        virtual void OnConnect();
        virtual void OnSend(const Buffer& buffer);
        virtual void OnRecv(uint8_t* data, int32_t len);
        //socket has closed
        virtual void OnError(const ErrorCode& error);

    private:
        HivePtr _hive;
        Socket _socket;
        uint8_t _recv_buffer[RECV_BUFF_SIZE];
        BufferQueue _pending_sends;
        std::atomic<bool> _error_state;
        std::atomic<bool> _connected;
        uint32_t _connection_id;
        Buffer recv_buf_;
        size_t buf_size_;
        size_t read_index_;
        size_t readable_size_;
        size_t write_index_;
        size_t writable_size_;
        std::atomic<uint32_t> _max_send_queue_size;

    private:
        void StartSend();
        void StartRecv();
        void StartError(const ErrorCode& error);
        void DispatchSend(Buffer buffer);
        void HandleConnect(const ErrorCode& error);
        void HandleSend(const ErrorCode& error, BufferQueue::iterator itr);
        void HandleRecv(const ErrorCode& error, int32_t actual_bytes);
        bool HasError() { return _error_state; }
        bool CheckError(const ErrorCode& error) { return (error || HasError() || _hive->IsStopped()); }
    };
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    class PacketProcessor
    {
    protected:
        virtual void ProcessPacket(const ConnectionPtr&, const uint8_t*, uint32_t) = 0;
        virtual void EncodePacket(const uint8_t*, uint32_t, Buffer&);
        virtual uint32_t DecodePacket(const ConnectionPtr&, const uint8_t*, uint32_t);
    };
    //-----------------------------------------------------------------------------

    //-----------------------------------------------------------------------------
    /*!
     * @class Acceptor
     */
    class Server : public std::enable_shared_from_this<Server>, private boost::noncopyable
    {
        friend class Hive;

    public:
        Server(int pool_size);
        virtual ~Server();

        const HivePtr& GetHive() { return _hive; }

        void Listen(const std::string& host, const std::string& port);

        void SyncRun();
        void Stop();

        ConnectionCallBacks callbacks;

    protected:
        virtual bool OnAccept(const ConnectionPtr&, const std::string&) { return true; }

    protected:
        HivePtr _hive;
        Acceptor _acceptor;
        std::atomic<bool> _error_state;
        uint32_t _connection_id;
        HivePool _pool;

    private:
        void StartAccept();
        void HandleAccept(const ErrorCode& error, const ConnectionPtr& connection);
        bool HasError() { return _error_state; }
        void StartError(const ErrorCode& error);
        bool CheckError(const ErrorCode& error) { return (error || HasError() || _hive->IsStopped()); }

    };
    //-----------------------------------------------------------------------------
}
