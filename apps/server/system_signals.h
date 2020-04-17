/**
 * @file system_signals.h
 * @brief provides interfaces for gracefull system signals handling, if possible.
 * 
 */

#pragma once

#include <stdatomic.h>

enum RunState
{
    RUN_STATE_STARTING,
    RUN_STATE_RUNNING,
    RUN_STATE_EXITING_GRACEFULLY,
};

extern volatile atomic_int run_state;

/**
 * @brief Sets up system signals handling for gracefull exit, if possible. E.g. SIGINT, SIGTERM, etc..
 * 
 */
void
setupSignalHandling();