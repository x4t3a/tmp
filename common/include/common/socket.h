#pragma once

#include <mbedtls/net_sockets.h>
#include <common/buffer_view.h>

typedef mbedtls_net_context SocketContext;

typedef struct __ConnectionOptions
{
    char const* interface;
    char const* port;
    int protocol;
} ConnectionOptions;

int
initSocketContext(SocketContext* socket_ctx);

int
freeSocketContext(SocketContext* socket_ctx);

int
acceptConnection(SocketContext* client_ctx, SocketContext* bind_ctx);

int
bindSocket(SocketContext* bind_ctx, ConnectionOptions* opts);

int
connectSocket(SocketContext* connect_ctx, ConnectionOptions* opts);

size_t
recvBufferized(
    BufferView*    buffer_view,
    SocketContext* client_ctx,
    bool           drop_packet,
    bool*          connection_interrupted
);

bool
sendBufferized(SocketContext* connect_ctx, BufferView* send_view);