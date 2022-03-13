#ifndef _NET_TOOLS_OPERATOR_IMPL_H_
#define _NET_TOOLS_OPERATOR_IMPL_H_

#include <unistd.h>
#include <cstdint>
#include <boost/system/error_code.hpp>
#include <boost/asio/detail/socket_ops.hpp>

#include <boost/asio/ip/tcp.hpp>
#include "operator_base.h"

namespace net_tools
{
using namespace boost::asio::detail;
using boost::asio::ip::tcp;

class reactive_socket_accept_op : public reactor_op
{
public:
    reactive_socket_accept_op(int socket, func_type complete_func)
        : reactor_op(&reactive_socket_accept_op::do_perform, complete_func),
        socket_(socket)
    {
    }

    // 需要循环调用，避免多个连接同时返回时，ET模式下只accept了一个连接
    static status do_perform(reactor_op* base)
    {
        reactive_socket_accept_op* o(
            static_cast<reactive_socket_accept_op*>(base));

        socket_type new_socket = invalid_socket;
        status result = socket_ops::non_blocking_accept(o->socket_,
            socket_ops::user_set_non_blocking | socket_ops::enable_connection_aborted,
            o->remote_endpoint_.data(), &o->remote_endpoint_.size(),
            o->ec_, new_socket)
        ? done : not_done;
        o->new_socket_ = new_socket;

        return result;
    }

    tcp::endpoint& get_remote_endpoint() { return remote_endpoint_; }

    int get_new_socket() { return new_socket_; }

private:
    int socket_;
    int new_socket_;
    tcp::endpoint  remote_endpoint_;
};

class reactive_socket_connect_op : public reactor_op
{
public:
    reactive_socket_connect_op(socket_type socket, func_type complete_func)
        : reactor_op(&reactive_socket_connect_op::do_perform, complete_func),
        socket_(socket)
    {
    }

    static status do_perform(reactor_op* base)
    {
        reactive_socket_connect_op* o(
            static_cast<reactive_socket_connect_op*>(base));

        status result = socket_ops::non_blocking_connect(
            o->socket_, o->ec_) ? done : not_done;

        return result;
    }

private:
    socket_type socket_;
};


class reactive_socket_read_op : public reactor_op
{
public:
    reactive_socket_read_op(int descriptor, func_type complete_func)
        : reactor_op(&reactive_socket_read_op::do_perform, complete_func),
        descriptor_(descriptor),
        buffers_(buffers)
    {
    }

    static bool non_blocking_read(int d, const char* bufs,
        boost::system::error_code& ec, std::size_t& bytes_transferred)
    {
        for (;;)
        {
            // Read some data.
            errno = 0;
            signed_size_type bytes = error_wrapper(::read(
                d, bufs, 4096), ec);

            // Check for end of stream.
            if (bytes == 0)
            {
                ec = boost::asio::error::eof;
                return true;
            }

            // Retry operation if interrupted by signal.
            if (ec == boost::asio::error::interrupted)
                continue;

            // Check if we need to run the operation again.
            if (ec == boost::asio::error::would_block
                || ec == boost::asio::error::try_again)
                return false;

            // Operation is complete.
            if (bytes > 0)
            {
                ec = boost::system::error_code();
                bytes_transferred = bytes;
            }
            else
                bytes_transferred = 0;

            return true;
        }
    }

    static status do_perform(reactor_op* base)
    {
        reactive_socket_read_op* o(static_cast<reactive_socket_read_op*>(base));

        buffer_sequence_adapter<boost::asio::mutable_buffer,
            MutableBufferSequence> bufs(o->buffers_);

        status result = descriptor_ops::non_blocking_read(o->descriptor_,
            bufs.buffers(), bufs.count(), o->ec_, o->bytes_transferred_)
        ? done : not_done;

        BOOST_ASIO_HANDLER_REACTOR_OPERATION((*o, "non_blocking_read",
            o->ec_, o->bytes_transferred_));

        return result;
    }

private:
    int descriptor_;
    MutableBufferSequence buffers_;
};

class reactive_socket_write_op : public reactor_op
{
public:
    reactive_socket_write_op(int descriptor, func_type complete_func)
        : reactor_op(&reactive_socket_write_op::do_perform, complete_func),
        descriptor_(descriptor),
        buffers_(buffers)
    {
    }

    static status do_perform(reactor_op* base)
    {
        reactive_socket_write_op* o(static_cast<reactive_socket_write_op*>(base));

        buffer_sequence_adapter<boost::asio::const_buffer,
            ConstBufferSequence> bufs(o->buffers_);

        status result = descriptor_ops::non_blocking_write(o->descriptor_,
            bufs.buffers(), bufs.count(), o->ec_, o->bytes_transferred_)
        ? done : not_done;

        BOOST_ASIO_HANDLER_REACTOR_OPERATION((*o, "non_blocking_write",
            o->ec_, o->bytes_transferred_));

        return result;
    }

private:
    int descriptor_;
    ConstBufferSequence buffers_;
};

}

#endif