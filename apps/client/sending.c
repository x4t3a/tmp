/**
 * @file sending.c
 * @brief contains client's logic implementation related to packets' construction from a file, and later sending those packets over network.
 * 
 */

#include "sending.h"

#include <common/protocol.h>
#include <common/hash.h>
#include <common/endiannes.h>
#include <common/timestamping.h>

#include <arpa/inet.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

#define USLEEP_10_MILLISECONDS 10000
#define SLEEP_10_SECONDS 10

/**
 * @brief Helper function, which parses a line of comma separated integers, and puts them as 2-byte ints into \p packet payload section in network order.
 * 
 * @param packet[out] is a pointer whose data field is going to be filled.
 * @param line[in] is a line to be parsed.
 * @param line_size[in] is currently unused.
 * @return size_t number of payload in raw bytes representation.
 */
static size_t
fillInPayload(Packet* packet, char* line, size_t line_size) // pass pointer to the whole packet, so there won't be unaligned access
{
    size_t words_parsed = 0;

    char const* separator = ",";
    char*       str_word  = strtok(line, separator);

    while (str_word)
    {
        uint16_t word = (uint16_t) atoi(str_word);
        packet->data.data[words_parsed] = htons(word);
        ++words_parsed;

        if (words_parsed >= MAX_PACKET_DATA_SIZE)
        {
            break;
        }

        str_word = strtok(NULL, separator); // NB: not thread-safe
    }

    return (words_parsed);
}

/**
 * @brief Helper for computing and filling (in host order) packet's hash: over all headers (except hash field itself) plus payload.
 * 
 * @param packet[out] pointer to the whole packet buffer, this helper uses its header.hash subfield, which is filled in.
 * @param payload_size[in] number of bytes in the payload.
 * @return true on error.
 * @return false on success.
 */
static bool
fillInHash(Packet* packet, DataSize payload_size)
{
    bool hash_error = calcMD5Hash(
        packet->header.hash,
        &(BufferView){
            (unsigned char*) &(packet->header.timestamp),
            sizeof(PacketHeader) - sizeof(Hash) + payload_size
        }
    );

    htond(packet->header.hash, sizeof(Hash));

    return hash_error;
}

/**
 * @brief Helper for filling in \p packet header subfield.
 * 
 * @param packet[out] pointer to the whole packet buffer, this helper uses its header subfield, which is filled in.
 * @param packet_number[out] pointer to return assigned packet number.
 * @param payload_size[in] number of bytes in the payload.
 * @return true on error.
 * @return false on success.
 */
static bool
fillInHeader(Packet* packet, PacketNumber* packet_number, DataSize payload_size)
{
    static PacketNumber packet_no = 0;

    MicroSecondsTimeStamp time_stamp = getMicroSecondsTimeStamp();
    packet->header.timestamp = htonll(time_stamp);
    packet->header.packet_no = htonl(packet_no);
    packet->header.data_size = htons(payload_size);

    bool hash_error = fillInHash(packet, payload_size);
    if (hash_error)
    {
        return true;
    }

    {
        time_t ts_seconds = (time_stamp / MICROSECONDS_MULTIPLIER);
        char const* format = "%H:%M:%S";
        struct tm lt = {0};
        char date_buffer[32] = {0};
        (void) localtime_r(&ts_seconds, &lt);
        (void) strftime(date_buffer, sizeof(date_buffer), format, &lt);
        printf("Sent: %" PRIu32 ", %s, timestamp %" PRIu64  " (microseconds)\n", packet_no, date_buffer, time_stamp);
    }

    ++packet_no;
    *packet_number = packet_no;

    return false;
}

/**
 * @brief Helper for iterating \p file_ctx line by line, forming packets from them, and sending, using \p connect_ctx
 * 
 * @param connect_ctx[in] socket context used for sending the packets.
 * @param file_ctx[in] file context for parsing.
 * @param packet[in] pointer to buffer, which is filled, and then sent.
 * @return true on error.
 * @return false on success.
 */
static bool
sendLines(SocketContext* connect_ctx, FILE* file_ctx, Packet* packet)
{
    char*  line      = NULL;
    size_t line_len  = 0;
    bool   got_error = false;

    while (true)
    {
        ssize_t read = getline(
            &line, // re/allocated automatically
            &line_len,
            file_ctx
        );

        if (!~read)
        {
            break;
        }

        DataSize payload_size = (DataSize) fillInPayload(packet, line, line_len);
        if (payload_size > 0)
        {
            PacketNumber assigned_packet_number = 0;
            bool header_error = fillInHeader(packet, &assigned_packet_number, payload_size);
            if (header_error)
            {
                got_error = true;
                fputs("FILE: couldn't fill in packet's header!!!", stderr);
                break;
            }
            else
            {
                int send_error = sendBufferized(
                    connect_ctx,
                    &(BufferView){
                        (unsigned char*) packet,
                        (sizeof(PacketHeader) + (sizeof(DataItem) * payload_size))
                    }
                );

                if (send_error)
                {
                    fprintf(stderr, "NET: sending error: %d!!!", send_error);
                    got_error = true;
                    break;
                }

                if (0 == (assigned_packet_number % 100))
                {
                    sleep(SLEEP_10_SECONDS);
                }
                else
                {
                    usleep(USLEEP_10_MILLISECONDS);
                }
            }
        }
        else
        {
            got_error = true;
            fputs("FILE: couldn't fill in packet's payload!!!", stderr);
            break;
        }
    } // while

    if (line)
    {
        free(line);
    }

    return got_error;
}

bool
sendFile(SocketContext* connect_ctx, char const* sent_file_path)
{
    FILE* file_ctx = fopen(sent_file_path, "r");
    if (NULL == file_ctx)
    {
        return true;
    }

    Packet packet_buffer = {0};
    bool send_error = sendLines(connect_ctx, file_ctx, &packet_buffer);

    fclose(file_ctx);

    return send_error;
}