/**
 * @file main.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#include <tftp.hpp>
#include "tftp_client.hpp"

int main()
{
    std::unique_ptr<YB::TFTPClient> client{new YB::TFTPClient()};

    bool status = client->create_socket();

    if (!status)
    {
        client.reset(client.get());
        return 1;
    }

    std::string file_name = "the_file_name_you_want_to_send_or_pull.txt";
    std::string directory = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)";

    // If you want to send a file to Server use below.
    client->send_data(directory + file_name);

    // If you want to pull a file from Server use below.
    // client->receive_data(directory + file_name);

    return 0;
}

/* end of file */
