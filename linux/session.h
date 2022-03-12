#ifndef _NET_TOOLS_SESSION_H_
#define _NET_TOOLS_SESSION_H_

#include <memory>
#include "stream.h"

namespace net_tools
{

#define BUF_LEN  4096

class Session
{
public:
    void read();
    void write();


private:
    char buffer_[BUF_LEN];
};

}

#endif
