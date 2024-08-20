/**
 * @file main.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#include <iostream>
#include <tftp_server.hpp>

int main()
{
    const std::unique_ptr<YB::TFTPServer> server{new YB::TFTPServer()};

    server->create_socket();
    server->bind_socket("127.0.0.1", 1234);

    // Server always awake with its file transfer directory
    const std::string root_dir = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)";
    server->wait_for_a_request(root_dir);

    return 0;
}

/* end of file */
