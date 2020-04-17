/**
 * @file app_options.c
 * @brief contains client app's arguments handling logic implementation.
 * 
 */

#include "app_options.h"

int
initAppOptions(ClientOptions* opts, char* argv[], int argc)
{
    if (opts && argv && (argc > 1))
    {
        opts->sent_file = argv[1];

        opts->connect.interface = (argc > 2)
            ? argv[2]
            : "127.0.0.1";

        opts->connect.port = (argc > 3)
            ? argv[3]
            : "8016";

        // mbedtls' interfaces look like it's possible to switch the transport this easy
        //opts->bind.protocol = ((argc > 1) && !strcmp(argv[3], "udp"))
        //    ? MBEDTLS_NET_PROTO_UDP
        //    : MBEDTLS_NET_PROTO_TCP;

        opts->connect.protocol = MBEDTLS_NET_PROTO_TCP;

        return 0;
    }

    return 1;
}
