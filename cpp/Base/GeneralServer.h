#pragma once

#include "network.h"

namespace Base
{
    class GeneralServer : public Server, public PacketProcessor
    {
    public:
        GeneralServer(const std::string& host, const std::string& port, int pool_size);
        virtual ~GeneralServer();

        void RegisterSignal(int sig, const Func& f) { _signals.add(sig); _signal_calls[sig] = f; }

    protected:
        virtual void ProcessPacket(const ConnectionPtr&, const uint8_t*, uint32_t) override;

    protected:
        virtual void OnConnect(const ConnectionPtr);
        virtual uint32_t OnRead(const ConnectionPtr&, const uint8_t*, uint32_t);
        virtual void OnWrite(const ConnectionPtr&, uint32_t);
        virtual void OnDisconnect(const ConnectionPtr);

    private:
        SignalSet _signals;
        std::unordered_map<int, Func> _signal_calls;

    private:
        void signalHanlder(const ErrorCode&e, int sig);
    };
}
