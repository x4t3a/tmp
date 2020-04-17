/**
 * @file sectioning.h
 * @brief provides interfaces for interaction with the buffer's subsections.
 * 
 */

#pragma once

#include <stdbool.h>
#include <sys/types.h>
#include <stdatomic.h>

enum SectionState
{
    S_UNUSED = 0,
    S_SERVED_BY_SOCKET,
    S_AWAITS_PACKETER,
    S_SERVED_BY_PACKETER
};

void
getFreeSubsectionNumber(
    ssize_t*               free_subsection,
    bool*                  drop,
    volatile atomic_uchar* subsections,
    size_t                 subsections_size
);

void
initializeSubsectionsMeta(
    volatile atomic_uchar* subsecitons,
    size_t                 subsections_size
);