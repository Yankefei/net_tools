#ifndef _NET_TOOLS_NET_SERVER_H_
#define _NET_TOOLS_NET_SERVER_H_

#include "stream.h"
#include <memory>
#include "string.hpp"

namespace net_tools
{

class Session;

class NetServerSpi
{
public:
    virtual void OnCreateSession(Session*) = 0;
    virtual void OnAccept(Session*) = 0;
    virtual void OnWriteData(Session*) = 0;
    virtual void OnClosed(Session*) = 0;
    virtual void OnHeartbeat(Session*) = 0;
};


class NetServerApi
{
public:
    static NetServerApi* CreateNetServerApi();

    virtual bool Register(NetServerSpi*) = 0;

    virtual bool Init(const tools::String&ip, uint16_t port, int max_session) = 0;

    virtual void Release() = 0;
};

}

#endif