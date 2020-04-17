/**
 * @file app_options.h
 * @brief contains server app's arguments handling logic interface.
 * 
 */

#pragma once

#include <common/socket.h>

#include <string.h>

typedef struct __ServerOptions
{
    ConnectionOptions bind; // bind options
} ServerOptions;

int
initAppOptions(ServerOptions* opts, char* argv[], int argc);