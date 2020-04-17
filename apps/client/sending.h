/**
 * @file sending.h
 * @brief contains client's logic interface related to packets' construction from a file, and later sending those packets over network.
 * 
 */

#pragma once

#include <common/socket.h>

/**
 * @brief Reads the \p sent_file_path file, creates a packet from each of the lines and sends those packets using \p connect_ctx socket context.
 * 
 * @param[in] connect_ctx is the socket context used for the packets sending.
 * @param[in] sent_file_path is the path to the file used for sending.
 * @return true if an error occured.
 * @return false if sending was successful.
 */
bool
sendFile(SocketContext* connect_ctx, char const* sent_file_path);