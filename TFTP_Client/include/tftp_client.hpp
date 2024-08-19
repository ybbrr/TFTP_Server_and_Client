///
/// @file tftp_client.hpp
/// @author Yasin BASAR
/// @brief
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP

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
    class TFTPClient
    {
    public:
    ////////////////////////////////////////////////////////////////////////////
    // Special Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPClient(TFTPClient &&) noexcept = default;
        TFTPClient &operator=(TFTPClient &&) noexcept = default;
        TFTPClient(const TFTPClient &) noexcept = delete;
        TFTPClient &operator=(TFTPClient const &) noexcept = delete;

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPClient();
        ~TFTPClient();
        void create_socket();
        void send_file(const std::string& file_path);
        void receive_file(const std::string& file_path);

    ////////////////////////////////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////////////////////////////////
    private:

        void send_ack_packet();

        void send_data_packet(int number_of_bytes_from_last_read);

        void send_rrq_packet(const std::string& file_name);

        void send_wrq_packet(const std::string& file_name);

        int receive_data_from_server();

        void send_transmission_done_signal();

        void close_socket_architecture() const;

        std::unique_ptr<char[]> m_incoming_buffer;
        std::unique_ptr<char[]> m_outgoing_buffer;

        SOCKET m_client_socket;
        SOCKADDR_IN m_server_info;
        SOCKADDR_IN m_peer;

        int m_addr_size;

    ////////////////////////////////////////////////////////////////////////////
    // Protected Members
    ////////////////////////////////////////////////////////////////////////////
    protected:

        // Data

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP

/* End of File */
