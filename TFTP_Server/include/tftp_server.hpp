///
/// @file tftp_server.hpp
/// @author Yasin BASAR
/// @brief This file contains the declaration of the TFTPServer class,
///        which handles incoming TFTP requests and sends/receives files.
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
    /// @class TFTPServer
    /// @brief The TFTPServer class provides methods for creating and binding
    ///        a socket, as well as handling incoming file transfer requests.
    class TFTPServer
    {
    public:
    ////////////////////////////////////////////////////////////////////////////
    // Special Members
    ////////////////////////////////////////////////////////////////////////////

        TFTPServer(TFTPServer &&) noexcept = default; ///< Default move constructor.
        TFTPServer &operator=(TFTPServer &&) noexcept = default; ///< Default move assignment operator.
        TFTPServer(const TFTPServer &) noexcept = delete; ///< Deleted copy constructor.
        TFTPServer &operator=(TFTPServer const &) noexcept = delete; ///< Deleted copy assignment operator.

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////////////////////////////////

        /// @brief Constructor for TFTPServer.
        TFTPServer();

        /// @brief Destructor for TFTPServer.
        ~TFTPServer();

        /// @brief Creates a socket for TFTP communication.
        void create_socket();

        /// @brief Binds the socket to a specific IP address and port.
        void bind_socket();

        /// @brief Waits for a TFTP request (RRQ or WRQ) and handles the file transfer.
        /// @param save_directory The directory where received files will be saved.
        void wait_for_a_request(const std::string& save_directory);

    ////////////////////////////////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////////////////////////////////
    private:

        /// @brief Sends an acknowledgment packet to the client.
        void send_ack_packet();

        /// @brief Sends a data packet to the client.
        /// @param number_of_bytes_from_last_read Number of bytes read from the file.
        void send_data_packet(int number_of_bytes_from_last_read);

        /// @brief Receives data from the client.
        /// @return The number of bytes received.
        int receive_data_from_client();

        /// @brief Closes the socket and cleans up the Windows Socket Architecture.
        void close_socket_architecture() const;

        std::unique_ptr<char[]> m_incoming_buffer; ///< Buffer for incoming data.
        std::unique_ptr<char[]> m_outgoing_buffer; ///< Buffer for outgoing data.

        SOCKET m_server_socket; ///< Server socket descriptor.
        SOCKADDR_IN m_server_info; ///< Server socket address information.
        SOCKADDR_STORAGE_LH m_server_storage; ///< Storage for server socket address.

        int m_addr_storage_size; ///< Size of the socket address structure.

    ////////////////////////////////////////////////////////////////////////////
    // Protected Members
    ////////////////////////////////////////////////////////////////////////////
    protected:

        // Data

    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_SERVER_HPP

/* End of File */
