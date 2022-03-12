#ifndef _NET_TOOLS_EPOLL_REACTOR_H_
#define _NET_TOOLS_EPOLL_REACTOR_H_

#include "stream.h"
#include "general.h"

#include "boost/asio/detail/eventfd_select_interrupter.hpp"

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <thread>

using namespace tools;

namespace net_tools
{

#define EPOLL_MAX_NUM  2048

class EpollReactor
{
public:
    EpollReactor() = default;
    ~EpollReactor()
    {
        if (epoll_fd_ != -1)
            close(epoll_fd_);
        if (timer_fd_ != -1)
            close(timer_fd_);
    }

    bool init()
    {
        do
        {
            epoll_fd_ = epoll_create(EPOLL_MAX_NUM);
            if (epoll_fd_ < 0)
                return false;

            thread_ = std::thread(std::bind(&EpollReactor::run, this));

            add_interrupt_opt();
            add_timer_opt();

        }while(false);

        return true;
    }


    bool modify_epoll_event(int socket, int op,  struct epoll_event* event)
    {
        if (epoll_ctl(epoll_fd_, op, socket, event) < 0)
        {
            assert(false);
            return false;
        }
        return true;
    }

    bool release()
    {
        interrupt();
        if (thread_.joinable())
            thread_.join();
    }

private:
    void run()
    {
        int32_t timer_index = 0;
        struct epoll_event array[128];
        int array_len = sizeof(array)/sizeof(struct epoll_event);
        while(true)
        {
            int num_event = epoll_wait(epoll_fd_, array, array_len, -1);
            for (int i = 0; i < array_len; ++i)
            {
                void* ptr = array[i].data.ptr;
                if (ptr == &interrupter_)
                {
                    stream << "interrupter, stop epoll wait" << std::endl;
                    return;
                }
                else if (ptr == &timer_fd_)
                {
                    uint64_t exp;
                    read(timer_fd_, &exp, sizeof(uint64_t));  // 需要读取timer_fd的数据，才能继续触发
                    stream <<"index: "<< ++timer_index  << ", on timer ...thread_id: "<< GET_THREAD_ID << std::endl;
                }
            }
        }
    }

    void add_timer_opt()
    {
        timer_fd_ = timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC);
        epoll_event ev = { 0, { 0 } };
        ev.events = EPOLLIN | EPOLLERR | EPOLLET;
        ev.data.ptr = &timer_fd_;

        struct timespec it_value{0, 0};
        clock_gettime(CLOCK_REALTIME, &it_value);
        it_value.tv_sec += 1;
        itimerspec inter_time{{1, 0}, it_value};  // 设置1s的定时周期
        if (timerfd_settime(timer_fd_, TFD_TIMER_ABSTIME, &inter_time, nullptr) < 0)
        {
            stream << "settime failed" << std::endl;
            return;
        }

        modify_epoll_event(timer_fd_, EPOLL_CTL_ADD, &ev);
    }

    void add_interrupt_opt()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = EPOLLIN | EPOLLERR | EPOLLET;
        ev.data.ptr = &interrupter_;
        modify_epoll_event(interrupter_.read_descriptor(), EPOLL_CTL_ADD, &ev);
    }

    // 退出函数
    void interrupt()
    {
        interrupter_.interrupt();
    }

private:
    int epoll_fd_{-1};

    int timer_fd_{-1};

    boost::asio::detail::eventfd_select_interrupter  interrupter_;

    std::thread thread_;
};

}

#endif
