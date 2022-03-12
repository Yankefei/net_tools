#include "server.h"

namespace net_tools
{

bool Server::Register(NetServerSpi* ptr)
{
    if (ptr == nullptr) return false;
    server_spi_ = ptr;
    return true;
}

bool Server::Init(const tools::String& ip, uint16_t port, int max_session)
{
    session_array_.reset(new Session[max_session]);
    epoll_mgr_ptr_.reset(new EpollMgr(2, 2));

    epoll_mgr_ptr_->init();

    return true;
}

void Server::Release()
{
    epoll_mgr_ptr_->release();
}

}