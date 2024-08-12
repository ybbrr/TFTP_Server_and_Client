/**
 * @file main.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#include <iostream>
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

    client->send_data(R"(C:\Users\Kp\Desktop\deneme.txt)");

    return 0;
}

/* end of file */
