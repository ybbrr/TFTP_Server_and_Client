///
/// @file tftp_client.hpp
/// @author Yasin BASAR
/// @brief This file contains the declaration of the TFTPClient class,
///        which is responsible for sending and receiving files using the TFTP protocol.
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
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Third Party Includes
////////////////////////////////////////////////////////////////////////////////

#include <tftp.hpp>

namespace YB
{
    /// @class TFTPClient
    /// @brief The TFTPClient class provides methods for sending and receiving
    ///        files using the TFTP protocol.
    class TFTPClient
    {
    public:
    ////////////////////////////////////////////////////////////////////////////
    // Special Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPClient(TFTPClient &&) noexcept = default; ///< Default move constructor.
        TFTPClient &operator=(TFTPClient &&) noexcept = default; ///< Default move assignment operator.
        TFTPClient(const TFTPClient &) noexcept = delete; ///< Deleted copy constructor.
        TFTPClient &operator=(TFTPClient const &) noexcept = delete; ///< Deleted copy assignment operator.

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////////////////////////////////

        /// @brief Constructor for TFTPClient.
        TFTPClient();

        /// @brief Destructor for TFTPClient.
        ~TFTPClient();

        /// @brief Creates a socket for TFTP communication.
        void create_socket();

        /// @brief Sends a file to the TFTP server.
        /// @param file_path The path to the file to be sent.
        void send_file(const std::string& file_path);

        /// @brief Receives a file from the TFTP server.
        /// @param file_path The path to save the received file.
        void receive_file(const std::string& file_path);

    ////////////////////////////////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////////////////////////////////
    private:

        /// @brief Sends an acknowledgment packet to the server.
        void send_ack_packet();

        /// @brief Sends a data packet to the server.
        /// @param number_of_bytes_from_last_read Number of bytes read from the file.
        void send_data_packet(int number_of_bytes_from_last_read);

        /// @brief Sends a read request (RRQ) packet to the server.
        /// @param file_name The name of the file to be requested.
        void send_rrq_packet(const std::string& file_name);

        /// @brief Sends a write request (WRQ) packet to the server.
        /// @param file_name The name of the file to be written.
        void send_wrq_packet(const std::string& file_name);

        /// @brief Receives data from the server.
        /// @return The number of bytes received.
        int receive_data_from_server();

        /// @brief Sends a signal indicating that the transmission is done.
        void send_transmission_done_signal();

        /// @brief Closes the socket and cleans up the Windows Socket Architecture.
        void close_socket_architecture() const;

        std::unique_ptr<char[]> m_incoming_buffer; ///< Buffer for incoming data.
        std::unique_ptr<char[]> m_outgoing_buffer; ///< Buffer for outgoing data.

        SOCKET m_client_socket; ///< Client socket descriptor.
        SOCKADDR_IN m_server_info; ///< Server socket address information.
        SOCKADDR_IN m_peer; ///< Peer socket address information.

        int m_addr_size; ///< Size of the socket address structure.

    ////////////////////////////////////////////////////////////////////////////
    // Protected Members
    ////////////////////////////////////////////////////////////////////////////
    protected:

        // Data

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_CLIENT_HPP

/* End of File */
