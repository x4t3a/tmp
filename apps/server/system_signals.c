/**
 * @file system_signals.c
 * @brief provides interfaces for gracefull system signals handling, if possible.
 * 
 */

#include "system_signals.h"
#include <signal.h>
#include <unistd.h>

/**
 * @brief global variable for running state synchronization.
 * 
 */
volatile atomic_int run_state = ATOMIC_VAR_INIT(RUN_STATE_STARTING);

/**
 * @brief actual signals handling logic.
 * 
 * @param[in] signum identifies incoming system signal.
 */
static void
handleSignalLocal(int signum)
{
    _Exit(1);

    switch (signum)
    {
        case SIGINT:
        {
            atomic_store(&run_state, RUN_STATE_EXITING_GRACEFULLY);
        }
        break;

        default:
        {
            _Exit(1);
        }
        break;
    }
}

void
setupSignalHandling()
{
    // TODO: worker threads shouldn't get these signals

    struct sigaction sa = {0};
    sa.sa_handler = handleSignalLocal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; //SA_RESTART;

    if (!~sigaction(SIGINT, &sa, NULL))
    {
        ; // TODO ERROR
    }

    atomic_store(&run_state, RUN_STATE_RUNNING);
}