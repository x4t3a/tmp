#include <common/socket.h>
#include <common/protocol.h>
#include <string.h>
#include <arpa/inet.h>

int
initSocketContext(SocketContext* socket_ctx)
{
    if (socket_ctx)
    {
        mbedtls_net_init(socket_ctx);
        return 0;
    }

    return 1;
}

int
freeSocketContext(SocketContext* socket_ctx)
{
    if (socket_ctx)
    {
        mbedtls_net_free(socket_ctx);
        return 0;
    }

    return 1;
}

int
acceptConnection(SocketContext* client_ctx, SocketContext* bind_ctx)
{
    if (client_ctx && bind_ctx)
    {
        initSocketContext(client_ctx);

        int accept_error = mbedtls_net_accept(
            bind_ctx,
            client_ctx,
            NULL, // client ip output buffer: don't need for now
            0,    // client ip output buffer length
            NULL  // client ip actual size written
        );

        return accept_error;
    }

    return 1;
}

int
bindSocket(SocketContext* bind_ctx, ConnectionOptions* opts)
{
    if (bind_ctx && opts)
    {
        int init_socket_error = initSocketContext(bind_ctx);
        if (init_socket_error)
        {
            return 1;
        }

        int bind_error = mbedtls_net_bind(bind_ctx, opts->interface, opts->port, opts->protocol);
        return bind_error;
    }

    return 1;
}

int
connectSocket(SocketContext* connect_ctx, ConnectionOptions* opts)
{
    initSocketContext(connect_ctx);

    int connect_error = mbedtls_net_connect(connect_ctx, opts->interface, opts->port, opts->protocol);
    return connect_error;
}


// TODO: this one is ugly: refactor it.
// TODO: check buffer_view and other params.
size_t
recvBufferized(
    BufferView*    buffer_view,
    SocketContext* client_ctx,
    bool           drop_packet,
    bool*          connection_interrupted
)
{
    #define LOCAL_PACKET_DATA_SIZE_FIELD_START (sizeof(Hash) + sizeof(MicroSeconds) + sizeof(PacketNumber))
    #define LOCAL_MINIMAL_BUFFER (LOCAL_PACKET_DATA_SIZE_FIELD_START + sizeof(DataSize) + sizeof(uint16_t))
    unsigned char start_buffer[LOCAL_MINIMAL_BUFFER];

    size_t whole_received = 0;
    {
        size_t recv_acc = 0;
        while (true)
        {
            int received = mbedtls_net_recv(client_ctx, start_buffer + recv_acc, LOCAL_MINIMAL_BUFFER - recv_acc);
            if (received < 0)
            {
                *connection_interrupted = true;
                break;
            }

            if (received == 0)
            {
                break;
            }

            recv_acc += received;
            if (0 == received || (recv_acc == LOCAL_MINIMAL_BUFFER))
            {
                break;
            }
        }

        whole_received += recv_acc;
    }

    DataSize data_size = sizeof(DataItem) * ntohs(*((DataSize*) (&start_buffer[LOCAL_PACKET_DATA_SIZE_FIELD_START])));
    if (drop_packet && data_size)
    {
        // Skipping/ignoring/removing data from the internal socket's buffer is a Linux-specific feature. Let's do it POSIX-confirming way.
        unsigned char vla_buff[data_size];
        size_t drop_acc = 0;
        while (true)
        {
            int received = mbedtls_net_recv(client_ctx, vla_buff + drop_acc, data_size - drop_acc);

            if (received < 0)
            {
                *connection_interrupted = true;
                break;
            }

            drop_acc += received;
            if (0 == received || (drop_acc == data_size))
            {
                break;
            }
        }
    }
    else
    {
        memcpy(buffer_view->view, start_buffer, LOCAL_MINIMAL_BUFFER);
        size_t data_recv_acc = 0;
        while (true)
        {
            int received = mbedtls_net_recv(client_ctx, buffer_view->view + data_recv_acc + LOCAL_MINIMAL_BUFFER, data_size + data_recv_acc);

            if (received < 0)
            {
                *connection_interrupted = true;
                break;
            }

            data_recv_acc += received;
            if (0 == received || data_recv_acc == data_size)
            {
                break;
            }
        }

        whole_received += data_recv_acc;
    }

    #undef LOCAL_PACKET_DATA_SIZE_FIELD_START
    #undef LOCAL_MINIMAL_BUFFER

    return whole_received;
}

bool
sendBufferized(SocketContext* connect_ctx, BufferView* send_view)
{
    bool view_error = checkBufferView(send_view);
    if (view_error)
    {
        return true;
    }

    size_t sent_acc = 0;
    while (sent_acc < send_view->size)
    {
        int sent = mbedtls_net_send(
            connect_ctx,
            send_view->view + sent_acc,
            send_view->size - sent_acc
        );

        if (!~sent)
        { return true; }

        sent_acc += (size_t) sent;
    }

    return false;
}