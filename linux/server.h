#ifndef _NET_TOOLS_SERVER_H_
#define _NET_TOOLS_SERVER_H_

#include "stream.h"
#include <memory>
#include <functional>

#include "epoll_mgr.h"
#include "session.h"
#include "net_server.h"
#include "operator_impl.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace net_tools
{

#define NULL_OFFSET  0xffffffff

/*
    设计的方案：使用固定session数组提前设定连接的上限，可以在广播以及遍历所有连接的过程中
    始终保持无锁化。提升运行速度。
*/
class Server : public NetServerApi
{
public:
    Server() = default;
    ~Server() = default;

    bool Register(NetServerSpi* ptr) override;

    bool Init(const tools::String& ip, uint16_t port, int max_session) override;

    void Release() override;

    // 广播数据，多线程模式下发
    bool send(const char* buffer, size_t len);

    bool send(Session* ptr, const char* buffer, size_t len);

    void accept();

private:
    void accept_complete_cb(void*, operation*,
            const boost::system::error_code&, std::size_t);

private:
    int max_session_{0};
    std::unique_ptr<Session[]> session_array_{nullptr};

    std::unique_ptr<EpollMgr> epoll_mgr_ptr_;

    NetServerSpi* server_spi_{nullptr};

    int listen_socket_;

    tcp::endpoint  listen_endpoint_;
    std::unique_ptr<reactive_socket_accept_op>  accept_op_;
    std::unique_ptr<EpollContext> accept_context_;
};

}

#endif
