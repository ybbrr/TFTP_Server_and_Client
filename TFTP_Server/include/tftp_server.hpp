///
/// @file tftp_server.hpp
/// @author Yasin BASAR
/// @brief
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP

////////////////////////////////////////////////////////////////////////////////
// Project Includes
////////////////////////////////////////////////////////////////////////////////

#include <WinSock2.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Third Party Includes
////////////////////////////////////////////////////////////////////////////////

#include <tftp.hpp>

namespace YB
{
    class TFTPServer
    {
    public:
    ////////////////////////////////////////////////////////////////////////////
    // Special Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPServer(TFTPServer &&) noexcept = default;
        TFTPServer &operator=(TFTPServer &&) noexcept = default;
        TFTPServer(const TFTPServer &) noexcept = delete;
        TFTPServer &operator=(TFTPServer const &) noexcept = delete;

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPServer();
        ~TFTPServer();
        void create_socket();
        void bind_socket();
        void wait_for_a_request(const std::string& save_directory);

    ////////////////////////////////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////////////////////////////////
    private:

        void send_ack_packet();

        void send_data_packet(int number_of_bytes_from_last_read);

        int receive_data_from_client();

        void close_socket_architecture() const;

        std::unique_ptr<char[]> m_incoming_buffer;
        std::unique_ptr<char[]> m_outgoing_buffer;

        SOCKET m_server_socket;
        SOCKADDR_IN m_server_info;
        SOCKADDR_STORAGE_LH m_server_storage;

        int m_addr_storage_size;

    ////////////////////////////////////////////////////////////////////////////
    // Protected Members
    ////////////////////////////////////////////////////////////////////////////
    protected:

        // Data

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP

/* End of File */
