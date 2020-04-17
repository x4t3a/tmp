/**
 * @file app_options.h
 * @brief contains client app's arguments handling logic interface.
 * 
 */

#pragma once

#include <common/socket.h>

/**
 * @brief ClientOptions contains all possible client's arguments.
 * 
 */
typedef struct __ClientOptions
{
    ConnectionOptions connect;
    char const*       sent_file;
} ClientOptions;

/**
 * @brief Reads app's args and initializes the \p opts structure.
 * 
 * @param opts[out] is the argument structure to initialize.
 * @param argv[in] is app's argument strings vector.
 * @param argc[in] is app's arguments vector size.
 * @return int 0 on success.
 */
int
initAppOptions(ClientOptions* opts, char* argv[], int argc);