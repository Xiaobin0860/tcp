#include <iostream>
#include "../Base/GeneralClient.h"
#include "../Base/GeneralServer.h"

using namespace Base;

class MyServer : public GeneralServer
{
public:
    MyServer() : GeneralServer("0.0.0.0", "11112", 0) {}
    virtual ~MyServer()
    {
    }
    
protected:
    virtual void ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len) override
    {
        std::string msg((char*)data, len);
        std::cout << __FUNCTION__ << " msg=" << msg << std::endl;
        if (msg == "Close")
        {
            Stop();
            return;
        }
        std::string ret("World");
        Buffer buf;
        EncodePacket((uint8_t*)ret.c_str(), ret.size(), buf);
        conn->Send(buf);
    }
    virtual void OnConnect(const ConnectionPtr conn) override
    {
        std::cout << __FUNCTION__ << " id=" << conn->GetId() << std::endl;
    }
    virtual void OnDisconnect(const ConnectionPtr conn) override
    {
        std::cout << __FUNCTION__ << " id=" << conn->GetId() << std::endl;
    }
};
using ServerPtr = std::shared_ptr<MyServer>;

int main(int argc, char* argv[])
{
    ServerPtr s(new MyServer);
    s->SyncRun();
    
    return 0;
}
