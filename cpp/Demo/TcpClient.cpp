
#include "../Base/GeneralClient.h"
#include "../Base/GeneralServer.h"

using namespace Base;

class MyClient : public GeneralClient
{
public:
    MyClient() : GeneralClient() {
        Connect("127.0.0.1", "11112");
    }
    ~MyClient()
    {
    }
    
protected:
    virtual void ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len) override
    {
        std::string msg((char*)data, len);
        ++_msg_count;
        if (_msg_count > 10)
        {
            std::string hello("Close");
            SendPacket((uint8_t*)hello.c_str(), hello.size());
            _hive->Post([this] {
                Stop();
            });
        }
        else
        {
            std::string hello("Hello");
            SendPacket((uint8_t*)hello.c_str(), hello.size());
        }
    }
    virtual void OnConnect(const ConnectionPtr conn) override
    {
        std::string msg("Hello");
        SendPacket((uint8_t*)msg.c_str(), msg.size());
    }
    virtual void OnDisconnect(const ConnectionPtr conn) override
    {
    }
    
private:
    int _msg_count = 0;
};

int main(int argc, char* argv[])
{
    MyClient c;
    auto stop = [&c]() {
        c.Stop();
    };
    c.RegisterSignal(SIGINT, stop);
    c.RegisterSignal(SIGTERM, stop);
    c.RegisterSignal(SIGABRT, stop);
#if defined(SIGQUIT)
    c.RegisterSignal(SIGQUIT, stop);
#endif
    c.SyncRun();
    
    return 0;
}
