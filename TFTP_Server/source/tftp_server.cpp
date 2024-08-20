///
/// @file tftp_server.cpp
/// @author Yasin BASAR
/// @brief This file contains the implementation of the TFTPServer class methods.
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
#include "tftp_server.hpp"

////////////////////////////////////////////////////////////////////////////////
// Third Party Includes
////////////////////////////////////////////////////////////////////////////////

namespace YB
{
////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

    TFTPServer::TFTPServer()
        : m_incoming_buffer(new char[TFTP_INCOMING_DATA_BUFFER_LEN]),
          m_outgoing_buffer(new char[TFTP_OUTGOING_DATA_BUFFER_LEN]),
          m_server_socket{INVALID_SOCKET},
          m_server_info{},
          m_server_storage{},
          m_addr_storage_size{sizeof(SOCKADDR_STORAGE_LH)}
    {
#ifdef _WIN32
        WSADATA wsa_data{};

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

    TFTPServer::~TFTPServer()
    {
        this->close_socket_architecture();
    }

    void TFTPServer::create_socket()
    {
        this->m_server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (this->m_server_socket == SOCKET_ERROR)
        {
            const std::string error_str = "Error at socket creation. Error code: " +
                                          GET_LAST_ERROR();
            this->close_socket_architecture();

            throw std::runtime_error(error_str);
        }
    }

    void TFTPServer::bind_socket(const char* server_ip, int port)
    {
        this->m_server_info.sin_family = AF_INET;
        this->m_server_info.sin_port = htons(port);
#ifdef _WIN32
        this->m_server_info.sin_addr.S_un.S_addr = inet_addr(server_ip);
#endif
#ifdef __linux__
        this->m_server_info.sin_addr.s_addr = inet_addr(server_ip);
#endif
        std::memset(this->m_server_info.sin_zero, 0, sizeof(this->m_server_info.sin_zero));

        const int status = bind(this->m_server_socket,
                                reinterpret_cast<SOCKADDR*>(&this->m_server_info),
                                sizeof(this->m_server_info));

        if (status == SOCKET_ERROR)
        {
            const std::string error_str = "Error at binding. Error code: " +
                                          GET_LAST_ERROR();

            this->close_socket_architecture();

            throw std::runtime_error(error_str);
        }
    }

    void TFTPServer::wait_for_a_request(const std::string& save_directory)
    {
        //Wait for Request//
        int bytes = this->receive_data_from_client();

        if (this->m_incoming_buffer[1] == OP_CODE_WRQ)
        {
            TFTP::reset_ack_data_block_num();
            TFTP::reset_data_block_num();


            const std::string file_name(&this->m_incoming_buffer[2]);
            const std::string file_path = this->preferred_file_path(save_directory, file_name);

            std::ofstream out_file(file_path, std::ios::binary);

            if (!out_file.is_open())
            {
                this->close_socket_architecture();
                out_file.close();

                throw std::runtime_error("File could not be created for WRQ");
            }

            this->send_ack_packet();

            do
            {
                memset(this->m_incoming_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

                bytes = this->receive_data_from_client();

                out_file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

                this->send_ack_packet();
            }
            while (bytes >= TFTP_OUTGOING_DATA_BUFFER_LEN);

            out_file.close();

            return;
        }

        if (this->m_incoming_buffer[1] == OP_CODE_RRQ)
        {
            //Get RRQ//
            TFTP::reset_ack_data_block_num();
            TFTP::reset_data_block_num();

            const std::string file_name(&this->m_incoming_buffer[2]);
            const std::string file_path = this->preferred_file_path(save_directory, file_name);

            std::ifstream in_file(file_path, std::ios::binary);

            if (!in_file.is_open())
            {
                this->close_socket_architecture();
                in_file.close();

                throw std::runtime_error("File could not be found for RRQ");
            }

            memset(this->m_incoming_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
            //Create the file in the directory given//

            while (!in_file.eof())
            {
                memset(this->m_outgoing_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
                in_file.read(this->m_outgoing_buffer.get(), TFTP_OUTGOING_DATA_BUFFER_LEN);

                int number_of_bytes_from_last_read = static_cast<int>(in_file.gcount());

                this->send_data_packet(number_of_bytes_from_last_read);

                memset(this->m_outgoing_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

                (void)this->receive_data_from_client();

                if (this->m_incoming_buffer[1] != OP_CODE_ACK)
                {
                    this->close_socket_architecture();
                    throw std::runtime_error("ACK Packet of data is missing");
                }
            }

            in_file.close();

            return;
        }

        std::cout << "There is no RRQ or WRQ accepted. Quitting.\n";
        this->close_socket_architecture();
    }

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////

    void TFTPServer::send_ack_packet()
    {
        packet_t ack_packet = TFTP::make_ack_packet();

        (void)sendto(this->m_server_socket,
                     ack_packet.data_ptr.get(),
                     ack_packet.size,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_server_storage),
                     this->m_addr_storage_size);
    }

    void TFTPServer::send_data_packet(int number_of_bytes_from_last_read)
    {
        packet_t data_packet
            = TFTP::make_data_packet(this->m_outgoing_buffer.get());

        (void)sendto(this->m_server_socket,
                     data_packet.data_ptr.get(),
                     number_of_bytes_from_last_read + DATA_BEGIN,
                     0,
                     reinterpret_cast<SOCKADDR*>(&this->m_server_storage),
                     this->m_addr_storage_size);
    }

    int TFTPServer::receive_data_from_client()
    {
        return recvfrom(this->m_server_socket,
                        this->m_incoming_buffer.get(),
                        TFTP_INCOMING_DATA_BUFFER_LEN,
                        0,
                        reinterpret_cast<SOCKADDR*>(&this->m_server_storage),
                        &this->m_addr_storage_size);
    }

    void TFTPServer::close_socket_architecture() const
    {
        if (this->m_server_socket != INVALID_SOCKET)
        {
            CLOSE_SOCKET(this->m_server_socket);
        }

        CLEANUP();

        std::cout << "Socket Architecture is closed." << std::endl;
    }

    std::string TFTPServer::preferred_file_path(const std::string& save_directory,
                                                const std::string& file_name) const
    {
        std::string file_path{};

        if (save_directory[save_directory.length() - 1] == '/' ||
            save_directory[save_directory.length() - 1] == '\\')
        {
            file_path = save_directory + file_name;
        }
        else
        {
            file_path = save_directory + '/' + file_name;
        }

        std::replace(file_path.begin(), file_path.end(), '\\', '/');

        const std::filesystem::path path(file_path);
        std::filesystem::path canonical_path = std::filesystem::weakly_canonical(path);

        return canonical_path.make_preferred().string();
    }

////////////////////////////////////////////////////////////////////////////////
// Protected Functions
////////////////////////////////////////////////////////////////////////////////

} // YB

/* End of File */
