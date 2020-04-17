/**
 * @file client.c
 * @brief contains client's main function.
 *
 */

#include "app_options.h"
#include "sending.h"

#include <stdlib.h>

int
main(int argc, char* argv[])
{
    int main_exit_status = EXIT_SUCCESS;

    ClientOptions client_opts = {0};
    int options_error = initAppOptions(&client_opts, argv, argc);
    if (options_error)
    {
        main_exit_status = EXIT_FAILURE;
    }
    else
    {
        SocketContext connect_ctx = {0};
        int connect_error = connectSocket(
            &connect_ctx,
            &(client_opts.connect)
        );

        if (connect_error)
        {
            main_exit_status = EXIT_FAILURE;
        }
        else
        {
            int send_error = 0;

            while (!send_error)
            {
                send_error = sendFile(
                    &connect_ctx,
                    client_opts.sent_file
                );
            }

            main_exit_status = send_error ? EXIT_FAILURE : EXIT_SUCCESS;
        }

        freeSocketContext(&connect_ctx);
    }

    return main_exit_status;
}