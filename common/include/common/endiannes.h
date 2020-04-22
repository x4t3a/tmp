#pragma once

#include <stddef.h>
#include <stdint.h>

void
htond(void* data, size_t data_size);

void
ntohd(void* data, size_t data_size);

#ifdef __linux__
uint64_t
htonll(uint64_t data);

uint64_t
ntohll(uint64_t data);
#endif
