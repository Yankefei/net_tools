#ifndef _NET_TOOLS_UTIL_H_
#define _NET_TOOLS_UTIL_H_

#include "stream.h"

#include "fcntl.h"

namespace net_tools
{

#define LOG_DEBUG_ON 1
 
#ifdef LOG_DEBUG_ON 
#define LOG_DEBUG(fmt, args...) \
    do {  \
        printf("[DEBUG]:");\
        printf(fmt "\n", ##args); \
    } while(0);
#define LOG_INFO(fmt, args...) \
    do { \
        printf("[INFO]:");\
        printf(fmt "\n", ##args); \
    } while(0);
#define LOG_WARNING(fmt, args...) \
    do { \
        printf("[WARNING]:");\
        printf(fmt "\n", ##args); \
    } while(0);
#else
#define LOG_DEBUG(fmt, args...) 
#define LOG_INFO(fmt, args...) 
#define LOG_WARNING(fmt, args...) 
#endif
#define LOG_ERROR(fmt, args...) \
    do{ \
        printf("[ERROR]:");\
        printf(fmt "\n", ##args);\
    }while(0);


class Util
{
public:
    static int set_no_block_socket(int socket)
    {
        int old_socket_flag = ::fcntl(socket, F_GETFL, 0);
        int new_socket_flag = old_socket_flag | O_NONBLOCK;
        ::fcntl(socket, F_SETFL, new_socket_flag);
        return socket;
    }


};

}

#endif
