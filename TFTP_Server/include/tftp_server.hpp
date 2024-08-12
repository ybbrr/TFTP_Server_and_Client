/**
 * @file tftp_server.hpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP

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

    class TFTPServer
    {
    public:
    /***************************************************************************
     * Special Members
     **************************************************************************/

        TFTPServer(TFTPServer &&) noexcept = default;

        TFTPServer &operator=(TFTPServer &&) noexcept = default;

        TFTPServer(const TFTPServer &) noexcept = default;

        TFTPServer &operator=(TFTPServer const &) noexcept = default;

    /***************************************************************************
     * Public Members
     **************************************************************************/

        TFTPServer();
        ~TFTPServer();
        bool create_socket();
        bool bind_socket();
        void wait_for_a_request(const std::string& save_directory);

    /***************************************************************************
     * Private Members
     **************************************************************************/
    private:

        void close_socket_architecture() const;

        std::unique_ptr<YB::TFTP> m_tftp;

        std::vector<char> m_incoming_buffer;
        std::vector<char> m_outgoing_buffer;

        SOCKET m_server_socket;
        SOCKADDR_IN m_server_info;
        SOCKADDR_STORAGE_LH m_server_storage;

        int m_addr_size;

    /***************************************************************************
     * Protected Members
     **************************************************************************/
    protected:

        /* Data */

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP

/* End of File */
