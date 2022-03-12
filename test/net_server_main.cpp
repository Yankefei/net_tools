#include "stream.h"

#include "net_server.h"

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

    return 0;
}