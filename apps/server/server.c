/**
 * @file server.c
 * @brief contains server's main function.
 * 
 */

#include <stdlib.h>

#include "app_options.h"
#include "packeter.h"
#include "sectioning.h"
#include "socket_serving.h"
#include "system_signals.h"

int
main(int argc, char *argv[])
{
    int main_exit_status = EXIT_SUCCESS;

    ServerOptions app_opts = {0};
    int opts_init_error = initAppOptions(&app_opts, argv, argc);
    if (opts_init_error)
    {
        main_exit_status = EXIT_FAILURE;
    }
    else
    {
        setupSignalHandling();

        SocketContext bind_ctx = {0};
        int bind_error = bindSocket(&bind_ctx, &(app_opts.bind));
        if (bind_error)
        {
            main_exit_status = EXIT_FAILURE;
        }
        else
        {
            Packet packets_buffer[PACKETS_BUFFER_SIZE];
            int serve_error = acceptAndServe(
                packets_buffer,
                PACKETS_BUFFER_SIZE,
                &bind_ctx
            );

            if (serve_error)
            {
                main_exit_status = EXIT_FAILURE;
            }
        }

        (void) freeSocketContext(&bind_ctx);

        pthread_exit(NULL);
    }

    return main_exit_status;
}