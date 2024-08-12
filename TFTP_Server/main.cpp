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
    std::unique_ptr<YB::TFTPServer> server{new YB::TFTPServer()};

    bool status = server->create_socket();

    if (!status)
    {
        server.reset(server.get());
        return 1;
    }

    status = server->bind_socket();

    if (!status)
    {
        server.reset(server.get());
        return 1;
    }

    // Server always awake with its file transfer directory
    server->wait_for_a_request(R"(D:\ABC)");

    return 0;
}

/* end of file */
