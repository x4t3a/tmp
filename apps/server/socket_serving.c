/**
 * @file socket_serving.c
 * @brief provides core server's functionality.
 * 
 */

#include "packeter.h"
#include "socket_serving.h"
#include "sectioning.h"

#include <arpa/inet.h>
#include <pthread.h>
#include <stdatomic.h>

/**
 * @brief Helper function receiving data from \p client_ctx and signaling packeter on packet's completion.
 * 
 * @param[in] packeter_ctx is a context for synchronization and signaling with the packeter thread.
 * @param[in] packets_buffer shared packets buffer.
 * @param[in] sections size of the packets buffer, number of sections.
 * @param[in] client_ctx socket's context.
 */
static void
recvDataAndFeedItToPacketer(
    PacketerCtx*   packeter_ctx,
    Packet*        packets_buffer,
    size_t         sections,
    SocketContext* client_ctx
)
{
    bool connection_interrupted = false;
    
    while (!connection_interrupted)
    {
        bool drop = false;
        ssize_t subsection = -1;
        getFreeSubsectionNumber(
            &subsection,
            &drop,
            packeter_ctx->section_states,
            sections
        );

        if (true == drop)
        {
            fputs("DROP!!!DROP!!!DROP!!!DROP!!!DROP!!!DROP!!!DROP!!!DROP!!!", stderr);
        }

        if (false == drop)
        {
            atomic_store(&(packeter_ctx->section_states[subsection]), (unsigned char) S_SERVED_BY_SOCKET);
        }

        // TODO: here be dragons
        //   1. check for strict aliasing
        //   2. unaligned access
        unsigned char* current_packet_subbuffer = (unsigned char*) &(packets_buffer[subsection]);
        size_t msg_size = recvBufferized(
            &(BufferView){
                current_packet_subbuffer,
                sizeof(Packet)
            },
            client_ctx,
            drop,
            &connection_interrupted // connection error
        );

        if ((0 == msg_size) || connection_interrupted) // valid and not so valid exit
        {
            atomic_store(&(packeter_ctx->section_states[subsection]), (unsigned char) S_UNUSED);
            break;
        }

        Packet* packet = &packets_buffer[subsection];
        PacketNumber packet_no = ntohl(packet->header.packet_no);
        printf("Received: packet no: %d, section: %d, msg_size: %d\n", (int) packet_no, (int) subsection, (int) msg_size);

        if (false == drop)
        {
            atomic_store(&(packeter_ctx->section_states[subsection]), (unsigned char) S_UNUSED);
            handleRecvMsg(packeter_ctx, subsection);
        }
    }
}

int
acceptAndServe(
    Packet*        packets_buffer,
    size_t         packets_buffer_size,
    SocketContext* bind_ctx
)
{
    if (NULL == bind_ctx)
    { return 1; }

    PacketerCtx packeter_ctx = {
        .sync = {
            .cv       = PTHREAD_COND_INITIALIZER,
            .cv_mutex = PTHREAD_MUTEX_INITIALIZER
        },
        .packets_buffer = packets_buffer
    };

    initializeSubsectionsMeta(packeter_ctx.section_states, packets_buffer_size);

    pthread_t packeter_thread_handle;
    bool packeter_th_error = createPacketerThread(&packeter_thread_handle, &packeter_ctx);
    if (packeter_th_error)
    {
        return 1;
    }

    while (RUN_STATE_RUNNING == atomic_load(&run_state))
    {
        SocketContext client_ctx = {0};
        int accept_error = acceptConnection(&client_ctx, bind_ctx);

        if (accept_error)
        {
            // Nothing went wrong. Just received a signal while the accept call was in progress.
            // if (errno == EINTR) // `errno` isn't thread safe, workaround it with the `run_state` atomic
            if (RUN_STATE_RUNNING != atomic_load(&run_state))
            { break; } // exit gracefully
            else
            { return 1; } // complete FUBAR
        }

        recvDataAndFeedItToPacketer(&packeter_ctx, packets_buffer, packets_buffer_size, &client_ctx);

        freeSocketContext(&client_ctx);
    }

    pthread_join(packeter_thread_handle, NULL);

    return 0;
}
