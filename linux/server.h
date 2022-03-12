#ifndef _NET_TOOLS_SERVER_H_
#define _NET_TOOLS_SERVER_H_

#include "stream.h"
#include <memory>

#include "epoll_mgr.h"
#include "session.h"
#include "net_server.h"

namespace net_tools
{

#define NULL_OFFSET  0xffffffff

class Server : public NetServerApi
{
public:
    Server() = default;
    ~Server() = default;

    bool Register(NetServerSpi* ptr) override;

    bool Init(const tools::String& ip, uint16_t port, int max_session) override;

    void Release() override;

private:
    std::unique_ptr<Session[]> session_array_{nullptr};

    std::unique_ptr<EpollMgr> epoll_mgr_ptr_;

    NetServerSpi* server_spi_{nullptr};
};

}

#endif
