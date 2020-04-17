/**
 * @file packeter.h
 * @brief provides interfaces for interaction with the packeter: received packets handling functionality.
 * 
 */

#pragma once

#include "system_signals.h"

#include <common/protocol.h>
#include <common/socket.h>

#include <pthread.h>
#include <stdatomic.h>

#define PACKETS_BUFFER_SIZE 16

typedef struct __Sync
{
    pthread_cond_t  cv;
    pthread_mutex_t cv_mutex;
} Sync;

typedef struct __PacketerCtx
{
    Sync                  sync;
    volatile atomic_uchar section_states[PACKETS_BUFFER_SIZE];
    Packet*               packets_buffer;
} PacketerCtx;

void
handleRecvMsg(PacketerCtx* packet_ctx, size_t subsection);

void*
runPacketer(void* ctx);

bool
createPacketerThread(
    pthread_t*   packeter_thread_handle,
    PacketerCtx* packeter_ctx
);