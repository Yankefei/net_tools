#ifndef _NET_TOOLS_EPOLL_REACTOR_H_
#define _NET_TOOLS_EPOLL_REACTOR_H_

#include "stream.h"
#include "general.h"
#include "epoll_reactor_context.h"

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
    EpollReactor(int thread_num_for_queue)
        : thread_num_(thread_num_for_queue)
    {}

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
            for (int i = 0; i < max_ops; i++)
                op_queue_[i] = tools::create_mpmc_queue<OpItem>(4096);

            for (int i = 0; i < thread_num_; i++)
                thread_list_[i] = std::thread(std::bind(EpollReactor::pop_queue, this));

            epoll_fd_ = epoll_create(EPOLL_MAX_NUM);
            if (epoll_fd_ < 0)
                return false;

            thread_ = std::thread(std::bind(&EpollReactor::run, this));

            add_interrupt_opt();
            add_timer_opt();

        }while(false);

        return true;
    }

    bool release()
    {
        for (int i = 0; i < thread_num_; i++)
        {
            if (thread_list_[i].joinable())
                thread_list_[i].join();
        }

        for (int i = 0; i < max_ops; i++)
            tools::free_mpmc_queue(op_queue_[i]);

        interrupt();
        if (thread_.joinable())
            thread_.join();
    }

    tools::MPMCQueue<OpItem>** get_mpmc_queue_ptr()
    {
        return op_queue_;
    }

    /*
        将errno设置为线程局部变量是个不错的主意，事实上，GCC中就是这么干的。他保证了线程之间的
        错误原因不会互相串改，当你在一个线程中串行执行一系列过程，那么得到的errno仍然是正确的。
        
        显然，errno实际上，并不是我们通常认为的是个整型数值，而是通过整型指针来获取值的。这个整
        型就是线程安全的。
    */
    int register_descriptor(int descriptor, EpollContext* descriptor_data);

private:
    void run();

    void add_timer_opt();

    void add_interrupt_opt()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = EPOLLIN | EPOLLERR | EPOLLET;
        ev.data.ptr = &interrupter_;
        modify_epoll_event(interrupter_.read_descriptor(), EPOLL_CTL_ADD, &ev);
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

    // 退出函数
    void interrupt()
    {
        interrupter_.interrupt();
    }

    void pop_queue()
    {
        while(UNLIKELY(ACCESS_ONCE(stop_flag_)))
        {

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

private:
    int epoll_fd_{-1};

    int timer_fd_{-1};

    boost::asio::detail::eventfd_select_interrupter  interrupter_;

    std::thread thread_;           // for epoll_wait

    int thread_num_;
    std::thread*      thread_list_;  // for queue
    tools::MPMCQueue<OpItem>* op_queue_[max_ops]; // 多种类型的MPMC队列
};

}

#endif
