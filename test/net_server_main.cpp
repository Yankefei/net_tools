#include "stream.h"

#include "net_server.h"
#include <thread>
#include <chrono>

class TestServer : public net_tools::NetServerSpi
{
public:
    TestServer() = default;
    ~TestServer()
    {
        if (net_api_)
            net_api_->Release();
    }

    bool Init()
    {
        net_api_ = net_tools::NetServerApi::CreateNetServerApi();
        net_api_->Register(this);

        if (!net_api_->Init("127.0.0.1", 51114, 10))
            return false;
        
        return true;
    }

public:
    void OnCreateSession(net_tools::Session*) override
    {}
    void OnAccept(net_tools::Session*) override
    {}
    void OnWriteData(net_tools::Session*) override
    {}
    void OnClosed(net_tools::Session*) override
    {}
    void OnHeartbeat(net_tools::Session*) override
    {}

private:
    net_tools::NetServerApi* net_api_{nullptr};
};


int main()
{

    TestServer server;

    server.Init();

    int val = 10;
    while(val -- > 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}