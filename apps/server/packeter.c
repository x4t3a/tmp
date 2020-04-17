/**
 * @file packeter.h
 * @brief provides interfaces for interaction with the packeter: received packets handling functionality.
 * 
 */

#include "packeter.h"
#include "sectioning.h"

#include <common/timestamping.h>
#include <common/hash.h>
#include <common/endiannes.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void
handleRecvMsg(PacketerCtx* packet_ctx, size_t subsection)
{
    pthread_mutex_lock(&(packet_ctx->sync.cv_mutex));
    atomic_store(packet_ctx->section_states + subsection, (unsigned char) S_AWAITS_PACKETER);
    pthread_cond_signal(&(packet_ctx->sync.cv));
    pthread_mutex_unlock(&(packet_ctx->sync.cv_mutex));
}


static void
printHash(Hash hash)
{
    for (unsigned char i = 0; i < sizeof(Hash); ++i)
    {
        printf("%02X", (int) hash[i]);
    }
}

static void
printComparedHashes(Hash lhs, Hash rhs)
{
    printHash(lhs);
    printf(" <=> ");
    printHash(rhs);
    puts("");
}

static void
checkMsg(Packet* packets_buffer, size_t served_subsection_id)
{
    Packet* packet = (packets_buffer + served_subsection_id);

    DataSize data_size = ntohs(packet->header.data_size);

    Hash packet_hash = {0};

    PacketNumber packet_number = ntohl(packet->header.packet_no);
    DataSize payload_size = ntohs(packet->header.data_size);
    calcMD5Hash(
        packet_hash,
        &(BufferView){
            (unsigned char*) &(packet->header.timestamp),
            sizeof(PacketHeader) - sizeof(Hash) + payload_size
        }
    );

    ntohd(packet->header.hash, sizeof(Hash));
    bool hash_matches = (0 == memcmp(packet_hash, packet->header.hash, sizeof(Hash)));
    //printComparedHashes(packet_hash, packet->header.hash);

    MicroSecondsTimeStamp ms_ts = getMicroSecondsTimeStamp();

    printf(
        "Processed: packet_no: %" PRIu32 ", check ms timestamp: %" PRIu64 ", %s, psize: %d, section: %d\n",
        packet_number,
        ms_ts,
        (hash_matches ? "PASS" : "FAIL"),
        (int) payload_size,
        (int) served_subsection_id
    );
}

void*
runPacketer(void* ctx)
{
    PacketerCtx* packeter_ctx = (PacketerCtx*) ctx;
    size_t const packets_buffer_size = (sizeof(packeter_ctx->section_states) / sizeof(*(packeter_ctx->section_states)));

    while (RUN_STATE_RUNNING == atomic_load(&run_state))
    {
        pthread_mutex_lock(&(packeter_ctx->sync.cv_mutex));
        pthread_cond_wait(
            &(packeter_ctx->sync.cv),
            &(packeter_ctx->sync.cv_mutex)
        ); // implicitly unlocks then locks mutex

        while (true)
        {
            ssize_t served_subsection_id = -1;
            for (ssize_t served = 0; served < packets_buffer_size; ++served)
            {
                if (atomic_load(packeter_ctx->section_states + served) == (unsigned char) S_AWAITS_PACKETER)
                {
                    served_subsection_id = served;
                    break;
                }
            }

            if (-1 == served_subsection_id)
            {
                break;
            }
            else
            {
                //printf("serving: %d\n", (int) served_subsection_id);

                checkMsg(packeter_ctx->packets_buffer, (size_t) served_subsection_id);

                // TODO TIMEOUTS
                usleep(15000);

                atomic_store(packeter_ctx->section_states + served_subsection_id, (unsigned char) S_UNUSED);
            }
        }

        pthread_mutex_unlock(&(packeter_ctx->sync.cv_mutex));
    }

    pthread_exit(NULL);

    return NULL;
}

bool
createPacketerThread(
    pthread_t*   packeter_thread_handle,
    PacketerCtx* packeter_ctx
)
{
    int packeter_thread_creation_error = pthread_create(
        packeter_thread_handle,
        NULL,
        &runPacketer,
        (void*) packeter_ctx
    );

    return packeter_thread_creation_error;
}