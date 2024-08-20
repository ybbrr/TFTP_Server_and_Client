///
/// @file main.cpp
/// @author Yasin BASAR
/// @brief
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///

#include <tftp.hpp>
#include "tftp_client.hpp"

int main()
{
    const std::unique_ptr<YB::TFTPClient> client{new YB::TFTPClient()};

    client->create_socket("127.0.0.1", 1234);

    const std::string file_name = "name_of_the_file_you_want_to_send_or_pull.txt";;
    const std::string directory = R"(the\directory\for\you\to\send\the\file\or\save\the\file\)";

    // If you want to send a file to Server use below.
    client->send_file(directory + file_name);

    // If you want to pull a file from Server use below.
    //client->receive_file(directory + file_name);

    return 0;
}

/* end of file */
