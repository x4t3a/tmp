/**
 * @file socket_serving.h
 * @brief provides core server's functionality.
 * 
 */

#pragma once

#include <stddef.h>
#include <common/protocol.h>
#include <common/socket.h>

/**
 * @brief Core server's functionality: receiveing data from socket in one thread, working on it in another one.
 * 
 * @param[in] packets_buffer is a buffer for receiving packets.
 * @param[in] packets_buffer_size size of \p packets_buffer array.
 * @param[in] bind_ctx is a socket context thru which client connections are accepted and served.
 * @return int 0 on success.
 */
int
acceptAndServe(
    Packet*        packets_buffer,
    size_t         packets_buffer_size,
    SocketContext* bind_ctx
);