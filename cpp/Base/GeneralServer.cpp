#include "GeneralServer.h"

using namespace Base;

GeneralServer::GeneralServer(const std::string& host, const std::string& port, int pool_size)
: Server(pool_size)
, _signals(_hive->GetService())
{
    _signals.add(SIGABRT);
    _signals.add(SIGINT);
    _signals.add(SIGTERM);
#if defined(SIGPIPE)
    _signals.add(SIGPIPE);
#endif
#if defined(SIGQUIT)
    _signals.add(SIGQUIT);
#endif
    
    _signals.async_wait(std::bind(&GeneralServer::signalHanlder, this, std::placeholders::_1, std::placeholders::_2));
    
    Listen(host, port);
    
    callbacks = {
        std::bind(&GeneralServer::OnConnect, this, std::placeholders::_1),
        std::bind(&GeneralServer::OnRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&GeneralServer::OnWrite, this, std::placeholders::_1, std::placeholders::_2),
        std::bind(&GeneralServer::OnDisconnect, this, std::placeholders::_1)
    };
}
GeneralServer::~GeneralServer()
{
}

void GeneralServer::ProcessPacket(const ConnectionPtr& conn, const uint8_t*, uint32_t len)
{
}

void GeneralServer::OnConnect(const ConnectionPtr conn)
{
}
uint32_t GeneralServer::OnRead(const ConnectionPtr& conn, const uint8_t* data, uint32_t len)
{
    return DecodePacket(conn, data, len);
}

void GeneralServer::OnWrite(const ConnectionPtr& conn, uint32_t len)
{
}

void GeneralServer::OnDisconnect(const ConnectionPtr conn)
{
}

void GeneralServer::signalHanlder(const ErrorCode&e, int sig)
{
    auto deal = [this](const ErrorCode& e, int sig) {
        if (e) return true;
#if defined(SIGPIPE)
        if (SIGPIPE == sig)
        {
            return true;
        }
#endif
        if (_signal_calls.find(sig) != _signal_calls.end())
        {
            _signal_calls[sig]();
            return true;
        }
        Stop();
        return false;
    };
    
    if (deal(e, sig))
    {
        _signals.async_wait(std::bind(&GeneralServer::signalHanlder, this, std::placeholders::_1, std::placeholders::_2));
    }
}
