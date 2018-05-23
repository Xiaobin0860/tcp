#pragma once

#include "network.h"

namespace Base
{
    class GeneralClient : public PacketProcessor
    {
    public:
        GeneralClient();
        virtual ~GeneralClient();

        void SendPacket(const uint8_t* data, uint32_t len);
        void SendPacket(const std::string& data)
        {
            SendPacket((uint8_t*)data.c_str(), (uint32_t)data.size());
        }

        void Connect(const std::string& host, const std::string& port);
        void Disconnect();

        void SyncRun();
        void AsyncRun();
        void Stop();

        const HivePtr& GetHive() { return _hive; }

        void RegisterSignal(int sig, const Func& f) { _signals.add(sig); _signal_calls[sig] = f; }

    protected:
        virtual void ProcessPacket(const ConnectionPtr&, const uint8_t*, uint32_t) override;

    protected:
        virtual void OnConnect(const ConnectionPtr);
        virtual uint32_t OnRead(const ConnectionPtr&, const uint8_t*, uint32_t);
        virtual void OnWrite(const ConnectionPtr&, uint32_t);
        virtual void OnDisconnect(const ConnectionPtr);

    protected:
        HivePtr _hive;
        ConnectionPtr _connection;
        std::atomic<bool> _running;
        std::atomic<bool> _connected;
        std::string _host;
        std::string _port;
        Timer _reconnect_timer;
        SignalSet _signals;
        std::unordered_map<int, Func> _signal_calls;

    private:
        void signalHanlder(const ErrorCode&e, int sig);
    };
}
