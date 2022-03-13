#include "server.h"

#include <sys/socket.h>
#include <sys/types.h>

using namespace boost::asio::ip;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

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
    epoll_mgr_ptr_.reset(new EpollMgr(2, 2));
    epoll_mgr_ptr_->init();

    max_session_ = max_session;
    session_array_.reset(new Session[max_session]);
    for (int i = 0; i < max_session; i++)
    {
        session_array_[i].init(epoll_mgr_ptr_->get_epoll_reactor());
        server_spi_->OnCreateSession(&session_array_[i], );
    }

    listen_endpoint_ = tcp::endpoint(address::from_string(ip.c_str()), port);

    listen_socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);
    ::bind(listen_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr));
    ::listen(listen_socket_, 10);

    // 关联epoll_rector 和 listen_socket_
    EpollReactor* rector_ptr = epoll_mgr_ptr_->get_epoll_reactor();
    accept_context_.reset(new EpollContext(rector_ptr, listen_socket_));
    rector_ptr->register_descriptor(listen_socket_, accept_context_.get());

    accept_op_.reset(new reactive_socket_accept_op(listen_socket_,
            std::bind(&Server::accept_complete_cb, this, accept_context_.get(), _1, _2, _3)));
    accept_context_->register_op_handle(accept_op_.get(), nullptr);

    return true;
}

void Server::Release()
{
    for (int i = 0; i < max_session_; i++)
        session_array_[i].release();

    epoll_mgr_ptr_->release();
}

bool Server::send(const char* buffer, size_t len)
{

}

bool Server::send(Session* ptr, const char* buffer, size_t len)
{

}

void Server::accept()
{

}

void Server::accept_complete_cb(void* ptr, operation* op_ptr,
        const boost::system::error_code& ec, std::size_t)
{
    reactive_socket_accept_op* op = static_cast<reactive_socket_accept_op*>(op_ptr);


}

}