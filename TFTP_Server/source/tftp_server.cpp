/**
 * @file tftp_server.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */


/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "tftp_server.hpp"

/*******************************************************************************
 * Third Party Libraries 
 ******************************************************************************/

namespace YB
{

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

    TFTPServer::TFTPServer()
        : m_tftp{new TFTP()},
          m_incoming_buffer(TFTP_INCOMING_DATA_BUFFER_LEN, 0),
          m_outgoing_buffer(TFTP_OUTGOING_DATA_BUFFER_LEN, 0),
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
            std::cout << "Error at Windows Socket Architecture initialization. Error code: " << WSAGetLastError() << "\n";
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
                          (SOCKADDR*)&this->m_server_info,
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
        int bytes = recvfrom(this->m_server_socket,
                             this->m_incoming_buffer.data(),
                             TFTP_INCOMING_DATA_BUFFER_LEN,
                             0,
                             (SOCKADDR*)&this->m_server_storage,
                             &this->m_addr_size);

        if (this->m_incoming_buffer[1] == OP_CODE_WRQ)
        {
            //Get WRQ//
            std::string file_name(&this->m_incoming_buffer[2]);

            file_name = save_directory + "\\" + file_name;

            std::ofstream out_file(file_name, std::ios::binary);

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

            std::vector<char> ack_packet = this->m_tftp->make_ack_packet();

            sendto(this->m_server_socket,
                   ack_packet.data(),
                   static_cast<int>(ack_packet.size()),
                   0,
                   (SOCKADDR*)&this->m_server_storage,
                   this->m_addr_size);

            do
            {
                memset(this->m_incoming_buffer.data(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

                bytes = recvfrom(this->m_server_socket,
                                 this->m_incoming_buffer.data(),
                                 TFTP_INCOMING_DATA_BUFFER_LEN,
                                 0,
                                 (SOCKADDR*)&this->m_server_storage,
                                 &this->m_addr_size);

                out_file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

                ack_packet.clear();
                ack_packet = this->m_tftp->make_ack_packet();
                sendto(this->m_server_socket,
                       ack_packet.data(),
                       static_cast<int>(ack_packet.size()),
                       0,
                       (SOCKADDR*)&this->m_server_storage,
                       this->m_addr_size);
            }
            while (bytes >= TFTP_OUTGOING_DATA_BUFFER_LEN);

            out_file.close();
            return;
        }
        else if (this->m_incoming_buffer[1] == OP_CODE_RRQ)
        {
            //Get RRQ//
            this->m_tftp->reset_ack_data_block_num();

            std::string file_name(&this->m_incoming_buffer[2]);

            file_name = save_directory + "\\" + file_name;

            std::ifstream in_file(file_name, std::ios::binary);

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

            memset(this->m_incoming_buffer.data(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
            //Create the file in the directory given//

            while (!in_file.eof())
            {
                memset(this->m_outgoing_buffer.data(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
                in_file.read(this->m_outgoing_buffer.data(), TFTP_OUTGOING_DATA_BUFFER_LEN);

                int number_of_bytes_from_last_read = static_cast<int>(in_file.gcount());

                std::vector<char> data_packet
                    = this->m_tftp->make_data_packet(this->m_outgoing_buffer.data());

                sendto(this->m_server_socket,
                       data_packet.data(),
                       number_of_bytes_from_last_read + DATA_BEGIN,
                       0,
                       (SOCKADDR*)&this->m_server_storage,
                       this->m_addr_size);

                memset(this->m_outgoing_buffer.data(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

                recvfrom(this->m_server_socket,
                         this->m_incoming_buffer.data(),
                         TFTP_INCOMING_DATA_BUFFER_LEN,
                         0,
                         (SOCKADDR*)&this->m_server_storage,
                         &this->m_addr_size);

                if (this->m_incoming_buffer[1] == OP_CODE_ACK)
                {
                    std::cout << "ACK accepted." << "\n";
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
        else
        {
            std::cout << "There is no RRQ or WRQ accepted. Quitting." << std::endl;
            this->close_socket_architecture();
            return;
        }
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

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Protected Functions
 ******************************************************************************/

} // YB

/* End of File */
