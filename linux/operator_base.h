#ifndef _NET_TOOLS_OPERATOR_BASE_H_
#define _NET_TOOLS_OPERATOR_BASE_H_

#include <cstdint>
#include <boost/system/error_code.hpp>

namespace net_tools
{

// Base class for all operations. A function pointer is used instead of virtual
// functions to avoid the associated overhead.
class operation
{
public:
  typedef operation operation_type;

  void complete(void* owner, const boost::system::error_code& ec,
      std::size_t bytes_transferred)
  {
      func_(owner, this, ec, bytes_transferred);
  }

  void destroy()
  {
      func_(0, this, boost::system::error_code(), 0);
  }

protected:
    typedef void (*func_type)(void*,
        operation*,
        const boost::system::error_code&, std::size_t);

    operation(func_type func):
        func_(func),
        task_result_(0)
    {
    }

    // Prevents deletion through this type.
    ~operation()
    {
    }

private:
    func_type func_;
    unsigned int task_result_; // Passed into bytes transferred.
};


// 具体的操作
class reactor_op : public operation
{
public:
  // The error code to be passed to the completion handler.
  boost::system::error_code ec_;

  // The number of bytes transferred, to be passed to the completion handler.
  std::size_t bytes_transferred_;

  // Status returned by perform function. May be used to decide whether it is
  // worth performing more operations on the descriptor immediately.
  enum status { not_done, done, done_and_exhausted };

  // Perform the operation. Returns true if it is finished.
  status perform()
  {
    return perform_func_(this);
  }

protected:
  typedef status (*perform_func_type)(reactor_op*);

  reactor_op(perform_func_type perform_func, func_type complete_func)
    : operation(complete_func),
      bytes_transferred_(0),
      perform_func_(perform_func)
  {
  }

private:
  perform_func_type perform_func_;
};


}

#endif