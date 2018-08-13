#include <iostream>
#include <boost/program_options.hpp>
#include "../Base/GeneralClient.h"

namespace po = boost::program_options;
using namespace Base;

class MyClient : public GeneralClient
{
public:
    MyClient(const std::string& host, const std::string& port) : GeneralClient() {
        Connect(host, port);
    }
    ~MyClient()
    {
    }
    
protected:
    virtual void ProcessPacket(const ConnectionPtr& conn, const uint8_t* data, uint32_t len) override
    {
        std::string msg((char*)data, len);
        std::cout << "ProcessPacket, recv " << msg << std::endl;
        ++_msg_count;
        if (_msg_count > 10)
        {
            std::string hello("Close");
            std::cout << "ProcessPacket, send " << hello << std::endl;
            SendPacket((uint8_t*)hello.c_str(), hello.size());
            _hive->Post([this] {
                Stop();
            });
        }
        else
        {
            std::string hello("Hello");
            std::cout << "ProcessPacket, send " << hello << std::endl;
            SendPacket((uint8_t*)hello.c_str(), hello.size());
        }
    }
    virtual void OnConnect(const ConnectionPtr conn) override
    {
        std::string msg("Hello");
        std::cout << "OnConnect " << conn->GetId() << ", send " << msg << std::endl;
        SendPacket((uint8_t*)msg.c_str(), msg.size());
    }
    virtual void OnDisconnect(const ConnectionPtr conn) override
    {
        std::cout << "OnDisconnect " << conn->GetId() << std::endl;
    }
    
private:
    int _msg_count = 0;
};

int main(int argc, char* argv[])
{
    std::string host;
    std::string port;
    po::options_description opts;
    opts.add_options()
        ("help", "produce help message")
        ("ip", po::value<std::string>(&host)->default_value("127.0.0.1"), "server ip")
        ("port", po::value<std::string>(&port)->default_value("11111"), "server port")
        ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, opts), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << opts << std::endl;
        return 1;
    }

    MyClient c(host, port);
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
