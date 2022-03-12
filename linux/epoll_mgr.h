#ifndef _NET_TOOLS_EPOLL_MGR_H_
#define _NET_TOOLS_EPOLL_MGR_H_

#include "stream.h"
#include "epoll_reactor.h"

#include <functional>
#include "lock_free_container/mpmc_queue.h"

namespace net_tools
{

/*
    epoll 的管理类
    epoll_num  参数表示初始化多少个epoll用于监控socket
    thread_num 参数表示使用多少线程来处理异步的读写
*/

enum class EventType
{
    WriteEvent,
    ReadEvent,

};

class EpollMgr
{
public:
    EpollMgr(int epoll_num, int thread_num) : epoll_num_(epoll_num)
    {
        reactor_ = new EpollReactor[epoll_num_];
    }

    ~EpollMgr()
    {
        if (reactor_)
            delete[] reactor_;
    }

    bool init()
    {
        for (int i = 0; i < epoll_num_; i++)
        {
            if (!reactor_[i].init())
                return false;
        }
    }

    void release()
    {
        for (int i = 0; i < epoll_num_; i++)
        {
            reactor_[i].release();
        }
    }

    void pop_queue()
    {
    }

private:
    struct EpollContext
    {
        //tools::MPMCQueue<> queue_;
    };

    struct epoll_context
    {
        struct epoll_event event_;
    }; 

    EpollReactor*     reactor_;
    int epoll_num_;
};

}

#endif