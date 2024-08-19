///
/// @file tftp_server.cpp
/// @author Yasin BASAR
/// @brief
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///


////////////////////////////////////////////////////////////////////////////////
// Project Includes
////////////////////////////////////////////////////////////////////////////////

#include "tftp_server.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

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
          m_server_socket{},
          m_server_info{},
          m_server_storage{},
          m_addr_size{}
    {
        WSADATA wsa_data{};
        WORD version = MAKEWORD(2, 2);
        int status = WSAStartup(version, &wsa_data);
        if (status != NO_ERROR)
        {
            std::cout << "Error at Windows Socket Architecture initialization."
                         "Error code: " << WSAGetLastError() << "\n";
            std::cout << "Press any key to quit..." << std::endl;
            system("pause");
            exit(0);
        }
        std::cout << "Windows Socket Architecture initialized." << "\n";
    }

    TFTPServer::~TFTPServer()
    {
        if (this->m_server_socket != INVALID_SOCKET)
        {
            closesocket(this->m_server_socket);
        }

        this->close_socket_architecture();
        std::cout << "Windows Socket Architecture is closed." << std::endl;
    }

    bool TFTPServer::create_socket()
    {
        this->m_server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (this->m_server_socket == SOCKET_ERROR)
        {
            std::cout << "Error at socket creating. Error code: " << WSAGetLastError() << std::endl;
            this->close_socket_architecture();
            return false;
        }

        return true;
    }

    bool TFTPServer::bind_socket()
    {
        this->m_server_info.sin_family = AF_INET;
        this->m_server_info.sin_port = htons(69);
        this->m_server_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        std::memset(this->m_server_info.sin_zero, 0, sizeof(this->m_server_info.sin_zero));

        int status = bind(this->m_server_socket,
                          reinterpret_cast<SOCKADDR*>(&this->m_server_info),
                          sizeof(this->m_server_info));

        if (status == SOCKET_ERROR)
        {
            std::cout << "Error at binding. Error code: " << WSAGetLastError() << std::endl;
            this->close_socket_architecture();
            return false;
        }

        this->m_addr_size = sizeof(this->m_server_storage);

        return true;
    }

    void TFTPServer::wait_for_a_request(const std::string &save_directory)
    {
        //Wait for Request//
        int bytes = this->receive_data_from_client();

        if (this->m_incoming_buffer[1] == OP_CODE_WRQ)
        {
            TFTP::reset_ack_data_block_num();
            TFTP::reset_data_block_num();

            //Get WRQ//
            std::string file_path(&this->m_incoming_buffer[2]);
            std::replace(file_path.begin(), file_path.end(), '\\', '/');
            file_path = save_directory + "/" + file_path;

            const std::filesystem::path path(file_path);
            std::filesystem::path canonical_path = std::filesystem::weakly_canonical(path);
            file_path = canonical_path.make_preferred().string();

            std::ofstream out_file(file_path, std::ios::binary);

            if (out_file.is_open())
            {
                std::cout << "File is open" << std::endl;
            }
            else
            {
                std::cout << "File not found. Quitting" << std::endl;
                this->close_socket_architecture();
                out_file.close();
                return;
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

            std::string file_path(&this->m_incoming_buffer[2]);
            std::replace(file_path.begin(), file_path.end(), '\\', '/');
            file_path = save_directory + "/" + file_path;

            const std::filesystem::path path(file_path);
            std::filesystem::path canonical_path = std::filesystem::weakly_canonical(path);
            file_path = canonical_path.make_preferred().string();

            std::ifstream in_file(file_path, std::ios::binary);

            if (in_file.is_open())
            {
                std::cout << "File is open" << std::endl;
            }
            else
            {
                std::cout << "File not found. Quitting" << std::endl;
                this->close_socket_architecture();
                in_file.close();
                return;
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

                if (this->m_incoming_buffer[1] == OP_CODE_ACK)
                {
                    std::cout << "ACK accepted.\n";
                }
                else
                {
                    std::cout << "Acknowledgement of data is missing."
                              << "Something went wrong between server and client."
                              << "Quitting." << std::endl;
                    this->close_socket_architecture();
                    return;
                }
            }
            in_file.close();
            return;
        }

        std::cout << "There is no RRQ or WRQ accepted. Quitting." << std::endl;
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
                     this->m_addr_size);
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
                     this->m_addr_size);
    }

    int TFTPServer::receive_data_from_client()
    {
        return recvfrom(this->m_server_socket,
                        this->m_incoming_buffer.get(),
                        TFTP_INCOMING_DATA_BUFFER_LEN,
                        0,
                        reinterpret_cast<SOCKADDR*>(&this->m_server_storage),
                        &this->m_addr_size);
    }

    void TFTPServer::close_socket_architecture() const
    {
        if (this->m_server_socket != INVALID_SOCKET)
        {
            closesocket(this->m_server_socket);
        }

        WSACleanup();

        std::cout << "Windows Socket Architecture is closed." << std::endl;
    }

////////////////////////////////////////////////////////////////////////////////
// Protected Functions
////////////////////////////////////////////////////////////////////////////////

} // YB

/* End of File */
