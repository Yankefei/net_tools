#include "net_server.h"
#include "server.h"

namespace net_tools
{

NetServerApi* NetServerApi::CreateNetServerApi()
{
    Server* impl_ptr = new Server();
    return static_cast<NetServerApi*>(impl_ptr);
}

}