/**
 * @file tftp_client.hpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>

/*******************************************************************************
 * Third Party Libraries
 ******************************************************************************/

#include <tftp.hpp>

namespace YB
{

    class TFTPClient
    {
    public:
    /***************************************************************************
     * Special Members
     **************************************************************************/

        TFTPClient(TFTPClient &&) noexcept = default;

        TFTPClient &operator=(TFTPClient &&) noexcept = default;

        TFTPClient(const TFTPClient &) noexcept = default;

        TFTPClient &operator=(TFTPClient const &) noexcept = default;

    /***************************************************************************
     * Public Members
     **************************************************************************/

        TFTPClient();
        ~TFTPClient();
        bool create_socket();
        void send_data(const std::string& file_path);
        void receive_data(const std::string& file_path);

    /***************************************************************************
     * Private Members
     **************************************************************************/
    private:

        void send_transmission_done_signal(const SOCKADDR* peer_addr);

        void close_socket_architecture() const;

        std::unique_ptr<YB::TFTP> m_tftp;

        std::vector<char> m_incoming_buffer;
        std::vector<char> m_outgoing_buffer;

        SOCKET m_client_socket;
        SOCKADDR_IN m_server_info;

        int m_addr_size;

    /***************************************************************************
     * Protected Members
     **************************************************************************/
    protected:

        /* Data */

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP

/* End of File */
