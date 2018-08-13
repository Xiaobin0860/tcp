#include <iostream>
#include <boost/program_options.hpp>
#include "../Base/GeneralClient.h"
#include "../Base/GeneralServer.h"

namespace po = boost::program_options;

using namespace Base;

class MyServer : public GeneralServer
{
public:
    MyServer(const std::string& port) : GeneralServer("0.0.0.0", port, 0) {}
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
        std::string ret(msg + " from c++ server");
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
    virtual void OnWrite(const ConnectionPtr& conn, uint32_t size)
    {
        std::cout << __FUNCTION__ << " id=" << conn->GetId() << ", write size " << size << std::endl;
    }
};
using ServerPtr = std::shared_ptr<MyServer>;

int main(int argc, char* argv[])
{
    std::string port;
    po::options_description opts;
    opts.add_options()
        ("help", "produce help message")
        ("port", po::value<std::string>(&port)->default_value("11111"), "listen port")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << std::endl;
        return 1;
    }

    ServerPtr s(new MyServer(port));
    s->SyncRun();
    
    return 0;
}
