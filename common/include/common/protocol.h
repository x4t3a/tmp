#pragma once

#include "hash.h"

#include <stdint.h>

typedef uint64_t MicroSeconds;
typedef uint32_t PacketNumber;

typedef uint16_t DataSize;

#define MAX_PACKET_DATA_SIZE 1600

typedef int16_t DataItem;
typedef struct  __attribute__((packed)) __Data
{
    DataItem data[MAX_PACKET_DATA_SIZE];
} Data;

typedef struct __attribute__((packed)) __PacketHeader
{
    Hash         hash;      // 16 bytes
    MicroSeconds timestamp; // 8  bytes
    PacketNumber packet_no; // 4  bytes
    DataSize     data_size; // 2  bytes
    uint16_t     reserved;  // 2  bytes
} PacketHeader; // 32 bytes

typedef struct __attribute__((packed)) __Packet
{
    PacketHeader header; // 32 bytes
    Data         data;   // (1600 * 2) 3200 bytes
} Packet;