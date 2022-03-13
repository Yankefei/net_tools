#include "session.h"

namespace net_tools
{

void Session::read()
{

}

void Session::write()
{

}

void Session::add_epoll_reactor(int socket)
{
   int res = reactor_->register_descriptor(socket, epoll_context_.get())
}

void Session::read_complete_cb(void*, operation*,
        const boost::system::error_code&, std::size_t)
{

}

void Session::write_complete_cb(void*, operation*,
        const boost::system::error_code&, std::size_t)
{

}

}
