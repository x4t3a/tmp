/**
 * @file sectioning.c
 * @brief provides interfaces for interaction with the buffer's subsections.
 * 
 */

#include "sectioning.h"

void
getFreeSubsectionNumber(
    ssize_t*               free_subsection,
    bool*                  drop,
    volatile atomic_uchar* subsections,
    size_t                 subsections_size
)
{
    ssize_t filled = -1;
    for (size_t subsection_id = 0; subsection_id < subsections_size; ++subsection_id)
    {
        // if (!~filled && (atomic_load(subsections + subsection_id) == (unsigned char) S_AWAITS_PACKETER))
        // {
        //     filled = (ssize_t) subsection_id;
        // }

        if (atomic_load(subsections + subsection_id) == (unsigned char) S_UNUSED)
        {
            *drop = false;
            *free_subsection = subsection_id;
            return;
        }
    }

    if (!~filled)
    {
        *drop = true;
    }

    *free_subsection = filled;
}

void
initializeSubsectionsMeta(
    volatile atomic_uchar* subsecitons,
    size_t                 subsections_size
)
{
    for (size_t subsection_id = 0; subsection_id < subsections_size; ++subsection_id)
    {   
        atomic_store(&(subsecitons[subsection_id]), (unsigned char) S_UNUSED);
    }
}