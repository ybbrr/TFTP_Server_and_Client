///
/// @file tftp_client.cpp
/// @author Yasin BASAR
/// @brief This file contains the implementation of the TFTPClient class methods.
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///

////////////////////////////////////////////////////////////////////////////////
// Project Includes
////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <filesystem>
#include "tftp_client.hpp"

////////////////////////////////////////////////////////////////////////////////
// Third Party Includes
////////////////////////////////////////////////////////////////////////////////

namespace YB
{
////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

    TFTPClient::TFTPClient()
        : m_incoming_buffer(new char[TFTP_INCOMING_DATA_BUFFER_LEN]),
          m_outgoing_buffer(new char[TFTP_OUTGOING_DATA_BUFFER_LEN]),
          m_client_socket{INVALID_SOCKET},
          m_server_info{},
          m_peer{},
          m_addr_size{sizeof(SOCKADDR_IN)}
    {
#ifdef _WIN32
        WSADATA wsa_data;
        constexpr WORD version = MAKEWORD(2, 2);
        const int status = WSAStartup(version, &wsa_data);

        if (status != NO_ERROR)
        {
            const std::string error_str
                = "Error at Windows Socket Architecture initialization. Error code: " +
                  std::to_string(WSAGetLastError());

            throw std::runtime_error(error_str);
        }
#endif
        std::cout << "Socket Architecture initialized.\n";
    }

    TFTPClient::~TFTPClient()
    {
        this->close_socket_architecture();
    }

    void TFTPClient::create_socket(const char* server_ip, int port)
    {
        this->m_client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (this->m_client_socket == SOCKET_ERROR ||
            this->m_client_socket == INVALID_SOCKET)
        {
            const std::string error_str = "Error at socket creation. Error code: " +
                                          GET_LAST_ERROR();
            this->close_socket_architecture();

            throw std::runtime_error(error_str);
        }

        this->m_server_info.sin_family = AF_INET;
        this->m_server_info.sin_port = htons(port);
#ifdef _WIN32
        this->m_server_info.sin_addr.S_un.S_addr = inet_addr(server_ip);
#endif
#ifdef __linux__
        this->m_server_info.sin_addr.s_addr = inet_addr(server_ip);
#endif
        memset(this->m_server_info.sin_zero, 0, this->m_addr_size);
    }

    void TFTPClient::send_file(const std::string& file_path)
    {
        memset(&this->m_peer, 0, this->m_addr_size);

        std::string file_path_ = file_path;
        std::replace(file_path_.begin(), file_path_.end(), '\\', '/');

        const std::filesystem::path path(file_path_);
        std::filesystem::path canonical_path = std::filesystem::weakly_canonical(path);

        file_path_ = canonical_path.make_preferred().string();
        const std::string file_name = canonical_path.filename().string();

        std::ifstream file(file_path_, std::ios::binary);

        if (!file.is_open())
        {
            this->close_socket_architecture();
            file.close();
            throw std::runtime_error("File could not be created for WRQ");
        }

        //send WRQ
        this->send_wrq_packet(file_name);

        //first ACK
        this->receive_data_from_server();

        if (this->m_incoming_buffer[1] != OP_CODE_ACK)
        {
            this->close_socket_architecture();
            this->send_transmission_done_signal();
            throw std::runtime_error("ACK Packet of data is missing");
        }

        while (!file.eof())
        {
            memset(this->m_outgoing_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
            file.read(this->m_outgoing_buffer.get(), TFTP_OUTGOING_DATA_BUFFER_LEN);
            int number_of_bytes_from_last_read = static_cast<int>(file.gcount());

            this->send_data_packet(number_of_bytes_from_last_read);

            memset(this->m_incoming_buffer.get(), 0, TFTP_INCOMING_DATA_BUFFER_LEN);

            this->receive_data_from_server();

            if (this->m_incoming_buffer[1] != OP_CODE_ACK)
            {
                this->close_socket_architecture();
                this->send_transmission_done_signal();
                throw std::runtime_error("ACK Packet of data is missing");
            }
        }

        file.close();
    }

    void TFTPClient::receive_file(const std::string& file_path)
    {
        memset(&this->m_peer, 0, this->m_addr_size);

        std::string file_path_ = file_path;
        std::replace(file_path_.begin(), file_path_.end(), '\\', '/');

        const std::filesystem::path path(file_path_);
        std::filesystem::path canonical_path = std::filesystem::weakly_canonical(path);

        file_path_ = canonical_path.make_preferred().string();
        std::string file_name = canonical_path.filename().string();

        std::ofstream file(file_path_, std::ios::binary);

        //send RRQ
        this->send_rrq_packet(file_name);

        //Get first data block
        int bytes = this->receive_data_from_server();

        if (this->m_incoming_buffer[1] != OP_CODE_DATA)
        {
            this->close_socket_architecture();
            file.close();
            this->send_transmission_done_signal();
            throw std::runtime_error("Data transfer could not start");
        }

        file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

        //send first ack packet
        this->send_ack_packet();

        while (bytes >= TFTP_OUTGOING_DATA_BUFFER_LEN)
        {
            memset(this->m_incoming_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

            bytes = this->receive_data_from_server();

            file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

            this->send_ack_packet();
        }

        file.close();
    }

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////

    void TFTPClient::send_ack_packet()
    {
        packet_t ack_packet = TFTP::make_ack_packet();

        (void)sendto(this->m_client_socket,
                     ack_packet.data_ptr.get(),
                     ack_packet.size,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_peer),
                     this->m_addr_size);
    }

    void TFTPClient::send_data_packet(int number_of_bytes_from_last_read)
    {
        packet_t data_packet
            = TFTP::make_data_packet(this->m_outgoing_buffer.get());

        (void)sendto(this->m_client_socket,
                     data_packet.data_ptr.get(),
                     number_of_bytes_from_last_read + DATA_BEGIN,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_peer),
                     this->m_addr_size);
    }

    void TFTPClient::send_rrq_packet(const std::string& file_name)
    {
        packet_t rrq_packet = TFTP::make_rrq_packet(file_name);

        (void)sendto(this->m_client_socket,
                     rrq_packet.data_ptr.get(),
                     rrq_packet.size,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_server_info),
                     this->m_addr_size);
    }

    void TFTPClient::send_wrq_packet(const std::string& file_name)
    {
        packet_t wrq_packet = TFTP::make_wrq_packet(file_name);

        (void)sendto(this->m_client_socket,
                     wrq_packet.data_ptr.get(),
                     wrq_packet.size,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_server_info),
                     this->m_addr_size);
    }

    int TFTPClient::receive_data_from_server()
    {
        return recvfrom(this->m_client_socket,
                        this->m_incoming_buffer.get(),
                        TFTP_INCOMING_DATA_BUFFER_LEN,
                        0,
                        reinterpret_cast<SOCKADDR*>(&this->m_peer),
                        &this->m_addr_size);
    }

    void TFTPClient::close_socket_architecture() const
    {
        if (this->m_client_socket != INVALID_SOCKET)
        {
            CLOSE_SOCKET(this->m_client_socket);
        }

        CLEANUP();

        std::cout << "Socket Architecture is closed." << std::endl;
    }

    void TFTPClient::send_transmission_done_signal()
    {
        const std::string done_str = "done";
        //tell the server the transmission is done.
        (void)sendto(this->m_client_socket,
                     done_str.c_str(),
                     done_str.length(),
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_peer),
                     this->m_addr_size);
    }

////////////////////////////////////////////////////////////////////////////////
// Protected Functions
////////////////////////////////////////////////////////////////////////////////

} // YB

/* End of File */
