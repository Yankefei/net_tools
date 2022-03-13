#include "epoll_reactor.h"

namespace net_tools
{

void EpollReactor::add_timer_opt()
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

/*
    将errno设置为线程局部变量是个不错的主意，事实上，GCC中就是这么干的。他保证了线程之间的
    错误原因不会互相串改，当你在一个线程中串行执行一系列过程，那么得到的errno仍然是正确的。
    
    显然，errno实际上，并不是我们通常认为的是个整型数值，而是通过整型指针来获取值的。这个整
    型就是线程安全的。
*/
int EpollReactor::register_descriptor(int descriptor, EpollContext* descriptor_data)
{
    epoll_event ev = { 0, { 0 } };
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    descriptor_data->registered_events_ = ev.events;
    ev.data.ptr = descriptor_data;
    int result = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
    if (result != 0)
    {
        if (errno == EPERM)
        {
            // This file descriptor type is not supported by epoll. However, if it is
            // a regular file then operations on it will not block. We will allow
            // this descriptor to be used and fail later if an operation on it would
            // otherwise require a trip through the reactor.
            descriptor_data->registered_events_ = 0;
            return 0;
        }
        return errno;
    }
}

void EpollReactor::run()
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
            else
            {
                EpollContext* context = static_cast<EpollContext*>(ptr);
                
                if (array[i].events & EPOLLIN)
                {
                    context->push_read_op();
                }
                else if (array[i].events & EPOLLOUT)
                {
                    context->push_write_op();
                }
            }
        }
    }
}



}
