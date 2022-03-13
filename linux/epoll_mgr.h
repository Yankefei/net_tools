#ifndef _NET_TOOLS_EPOLL_MGR_H_
#define _NET_TOOLS_EPOLL_MGR_H_

#include "stream.h"
#include "epoll_reactor.h"

#include <thread>
#include <chrono>
#include <functional>
#include "epoll_reactor_context.h"

namespace net_tools
{

/*
    epoll 的管理类
    epoll_num  参数表示初始化多少个epoll用于监控socket
    thread_num 参数表示使用多少线程来处理一个epoll所监控的的异步事件
*/

class EpollMgr
{
public:
    EpollMgr(int epoll_num, int thread_num_for_epoll)
        : epoll_num_(epoll_num)
    {
        reactor_list_.reset(new EpollReactor[epoll_num_](thread_num_for_epoll));
    }

    ~EpollMgr()
    {
    }

    bool init()
    {
        for (int i = 0; i < epoll_num_; i++)
        {
            if (!reactor_list_[i].init())
                return false;
        }
    }

    void release()
    {
        stop_flag_ = true;
        for (int i = 0; i < epoll_num_; i++)
        {
            reactor_list_[i].release();
        }
    }

    EpollReactor* get_epoll_reactor()
    {
        static int reactor_index = 0;
        return &reactor_list_[reactor_index++ % epoll_num_];
    }

private:
    std::unique_ptr<EpollReactor[]>  reactor_list_;

    int epoll_num_;
    bool stop_flag_{false};
};

}

#endif