/**
 * @file app_options.c
 * @brief contains server app's arguments handling logic interface.
 * 
 */

#include "app_options.h"

int
initAppOptions(ServerOptions* opts, char* argv[], int argc)
{
    if (opts && argv && argc)
    {
        opts->bind.interface = (argc > 1)
            ? argv[1]
            : "127.0.0.1";

        opts->bind.port = (argc > 2)
            ? argv[2]
            : "8016";

        // mbedtls' interfaces look like it's possible to switch the transport this easy
        //opts->bind.protocol = ((argc > 1) && !strcmp(argv[3], "udp"))
        //    ? MBEDTLS_NET_PROTO_UDP
        //    : MBEDTLS_NET_PROTO_TCP;

        opts->bind.protocol = MBEDTLS_NET_PROTO_TCP;

        return 0; // good
    }

    return 1; // error
}