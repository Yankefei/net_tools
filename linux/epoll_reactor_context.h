#ifndef _NET_TOOLS_EPOLL_REACTOR_CONTEXT_H_
#define _NET_TOOLS_EPOLL_REACTOR_CONTEXT_H_

#include "stream.h"
#include "lock_free_container/mpmc_queue.h"

#include "operator_base.h"

namespace net_tools
{

enum op_types { read_op = 0, write_op = 1,
    connect_op = 1, except_op = 2, max_ops = 3 };

class Session;
struct OpItem
{
    OpItem(EpollContext* context, op_types op)
        : context_(context), op_(op) {}

    EpollContext* context_;
    op_types      op_;
};

/*
    设计思路： Context是和某一个socket形成了一一对应的关系
*/
class EpollReactor;
struct EpollContext
{
    EpollContext(EpollReactor* reactor, int socket = 0)
        : reactor_(reactor), descriptor_(socket)
    {
        op_queue_ = reactor_->get_mpmc_queue_ptr();
    }

    ~EpollContext()
    {
    }

    void register_op_handle(reactor_op* read_op,
                            reactor_op* write_op)
    {
        read_op_ = read_op;
        write_op_ = write_op;
    }

    void push_read_op()
    {
        op_queue_[read_op]->push(OpItem(this, read_op));
    }

    void push_write_op()
    {
        op_queue_[write_op]->push(OpItem(this, write_op));
    }

    void push_except_op()
    {
        op_queue_[except_op]->push(OpItem(this, except_op));
    }

    EpollReactor* reactor_;
    int descriptor_;
    uint32_t registered_events_;
    tools::MPMCQueue<OpItem>** op_queue_; // 多种类型的MPMC队列
    reactor_op*   read_op_{nullptr};
    reactor_op*   write_op_{nullptr};
};

}

#endif