#ifndef _NET_TOOLS_SESSION_H_
#define _NET_TOOLS_SESSION_H_

#include <memory>
#include "stream.h"
#include "queue.h"

#include "epoll_reactor_context.h"
#include "epoll_reactor.h"

namespace net_tools
{

#define BUF_LEN  4096

enum class SessionStatus
{
    Uninitialized,
    Connecting,
    Accept,
    Connecting,
    closed
};


typedef int(*write_callback)(const char* buf, size_t len);
typedef int(*read_callback)(const char* buf, size_t len);
typedef int(*connect_callback)(const char* buf, size_t len);
typedef int(*accept_callback)(const char* buf, size_t len);
typedef int(*closed_callback)(const char* buf, size_t len);

class Session
{
public:
    Session() = default;

    ~Session() = default;

    void init(EpollReactor* reactor)
    {
        reactor_ = reactor;
        epoll_context_.reset(new EpollContext(reactor_));
        epoll_context_->register_op_handle(, );
    }

    void read();
    
    void write();

    void release() {}

private:
    // 连接启用后，需要将socket添加到epoll中管理
    void add_epoll_reactor(int socket);

    void read_complete_cb(void*, operation*,
            const boost::system::error_code&, std::size_t);

    void write_complete_cb(void*, operation*,
            const boost::system::error_code&, std::size_t);

private:
    char buffer_[BUF_LEN];

    int socket_;  // 可更新

    tools::Queue<OpItem> data_queue_;

    EpollReactor* reactor_{nullptr};
    std::unique_ptr<EpollContext>  epoll_context_;

    uint32_t  session_id_;

    std::unique_ptr<reactive_socket_accept_op>  accept_op_;
};

}

#endif
