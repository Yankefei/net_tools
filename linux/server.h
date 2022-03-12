#ifndef _NET_TOOLS_SERVER_H_
#define _NET_TOOLS_SERVER_H_

#include "stream.h"

#include "epoll_mgr.h"
#include "session.h"
#include "net_server.h"

namespace net_tools
{

class Server : public NetServerApi
{
public:
    bool Register(NetServerSpi* ptr) override;

    bool Init(const tools::String& ip, uint16_t port, int max_session) override;

    void Release() override;

private:
    Session* session_array_{nullptr};

    EpollMgr  epoll_mgr_;

    NetServerSpi* server_spi_{nullptr};
};

}

#endif
