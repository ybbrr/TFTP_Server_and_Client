/**
 * @file this->m_tftp_client.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */


/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "tftp_client.hpp"

/*******************************************************************************
 * Third Party Libraries 
 ******************************************************************************/

namespace YB
{

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

    TFTPClient::TFTPClient()
        : m_incoming_buffer(new char[TFTP_INCOMING_DATA_BUFFER_LEN]),
          m_outgoing_buffer(new char[TFTP_OUTGOING_DATA_BUFFER_LEN]),
          m_client_socket{},
          m_server_info{},
          m_addr_size{}
    {
        WSADATA wsa_data;
        WORD version = MAKEWORD(2, 2);
        int status = WSAStartup(version, &wsa_data);

        if (status != NO_ERROR)
        {
            std::cout << "Error at Windows Socket Architecture initialization. Error code: " << WSAGetLastError() << "\n";
            std::cout << "Press any key to quit..." << "\n";
            system("pause");
            exit(1);
        }

        std::cout << "Windows Socket Architecture initialized." << std::endl;
    }

    TFTPClient::~TFTPClient()
    {
        this->close_socket_architecture();
    }

    bool TFTPClient::create_socket()
    {
        this->m_client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (this->m_client_socket == SOCKET_ERROR)
        {
            std::cout << "Error at socket creating. Error code: " << WSAGetLastError() << std::endl;
            this->close_socket_architecture();
            return false;
        }

        this->m_server_info.sin_family = AF_INET;
        this->m_server_info.sin_port = htons(69);
        this->m_server_info.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        memset(this->m_server_info.sin_zero, 0, sizeof(this->m_server_info.sin_zero));

        this->m_addr_size = sizeof(this->m_server_info);

        return true;
    }

    void TFTPClient::send_data(const std::string& file_path)
    {
        size_t last_occurrence = file_path.find_last_of('/');

        std::string file_name{};

        if (last_occurrence != std::string::npos)
        {
            file_name = file_path.substr(last_occurrence + 1, file_path.length());
        }
        else
        {
            last_occurrence = file_path.find_last_of('\\');

            file_name = file_path.substr(last_occurrence + 1, file_path.length());
        }

        std::ifstream file(file_path, std::ios::binary);

        if (!file.is_open())
        {
            std::cout << "Directory not found" << "\n";
            this->close_socket_architecture();
            file.close();
            return;
        }

        SOCKADDR_IN peer_addr{};
        memset(&peer_addr, 0, sizeof(peer_addr));
        int len = sizeof(peer_addr);

        //send WRQ//
        packet_t wrq_packet
            = YB::TFTP::make_wrq_packet(file_name);

        sendto(this->m_client_socket,
               wrq_packet.data_ptr.get(),
               static_cast<int>(wrq_packet.size),
               0,
               (SOCKADDR*)&this->m_server_info,
               sizeof(SOCKADDR));
        //send WRQ//

        //first ACK
        recvfrom(this->m_client_socket,
                 this->m_incoming_buffer.get(),
                 TFTP_INCOMING_DATA_BUFFER_LEN,
                 0,
                 (SOCKADDR*)&peer_addr,
                 &len);

        if (this->m_incoming_buffer[1] == OP_CODE_ACK)
        {
            std::cout << "ACK accepted" << "\n";
        }
        else
        {
            std::cout << "Acknowledgement of data is missing."
                      << "Something went wrong between server and client."
                      << "Quitting." << std::endl;

            this->send_transmission_done_signal((SOCKADDR*)&peer_addr);

            return;
        }

        while (!file.eof())
        {
            memset(this->m_outgoing_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);
            file.read(this->m_outgoing_buffer.get(), TFTP_OUTGOING_DATA_BUFFER_LEN);
            int number_of_bytes_from_last_read = static_cast<int>(file.gcount());

            packet_t data_packet
                = YB::TFTP::make_data_packet(this->m_outgoing_buffer.get());

            sendto(this->m_client_socket,
                   data_packet.data_ptr.get(),
                   number_of_bytes_from_last_read + DATA_BEGIN,
                   0,
                   (SOCKADDR *) &peer_addr,
                   sizeof(SOCKADDR_IN));

            memset(this->m_incoming_buffer.get(), 0, TFTP_INCOMING_DATA_BUFFER_LEN);

            recvfrom(this->m_client_socket,
                     this->m_incoming_buffer.get(),
                     TFTP_INCOMING_DATA_BUFFER_LEN,
                     0,
                     (SOCKADDR *) &peer_addr,
                     &len);

            if (this->m_incoming_buffer[1] == OP_CODE_ACK)
            {
                std::cout << "ACK accepted. Data packet block_number: "
                          << data_packet.data_block_number << "\n";
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

        file.close();
    }

    void TFTPClient::receive_data(const std::string& file_path)
    {
        size_t last_occurrence = file_path.find_last_of('/');

        std::string file_name{};

        if (last_occurrence != std::string::npos)
        {
            file_name = file_path.substr(last_occurrence + 1, file_path.length());
        }
        else
        {
            last_occurrence = file_path.find_last_of('\\');

            file_name = file_path.substr(last_occurrence + 1, file_path.length());
        }

        std::ofstream file(file_path, std::ios::binary);

        //send RRQ//
        packet_t wrq_packet
            = YB::TFTP::make_rrq_packet(file_name);

        sendto(this->m_client_socket,
               wrq_packet.data_ptr.get(),
               static_cast<int>(wrq_packet.size),
               0,
               (SOCKADDR*)& this->m_server_info,
               this->m_addr_size);
        //send RRQ//

        SOCKADDR_IN server{};
        int len = sizeof(server);

        //Get first data block//
        int bytes = recvfrom(this->m_client_socket,
                             this->m_incoming_buffer.get(),
                             TFTP_INCOMING_DATA_BUFFER_LEN,
                             0,
                             (SOCKADDR*)&server,
                             &len);

        if (this->m_incoming_buffer[1] == OP_CODE_DATA)
        {
            std::cout << "Data packet accepted" << "\n";
        }
        else
        {
            std::cout << "Data packet is not accepted. Quitting." << "\n";
            this->close_socket_architecture();
            file.close();
            return;
        }

        file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

        //Get first data block//
        packet_t ack_packet = YB::TFTP::make_ack_packet();

        sendto(this->m_client_socket,
               ack_packet.data_ptr.get(),
               static_cast<int>(ack_packet.size),
               0,
               (SOCKADDR*)&server, len);

        while (bytes >= TFTP_OUTGOING_DATA_BUFFER_LEN)
        {
            memset(this->m_incoming_buffer.get(), 0, TFTP_OUTGOING_DATA_BUFFER_LEN);

            bytes = recvfrom(this->m_client_socket,
                             this->m_incoming_buffer.get(),
                             TFTP_INCOMING_DATA_BUFFER_LEN,
                             0,
                             (SOCKADDR*)&server,
                             &len);

            file.write(&this->m_incoming_buffer[DATA_BEGIN], bytes - DATA_BEGIN);

            //ack_packet.clear();
            ack_packet = YB::TFTP::make_ack_packet();

            sendto(this->m_client_socket,
                   ack_packet.data_ptr.get(),
                   static_cast<int>(ack_packet.size),
                   0,
                   (SOCKADDR*)&server,
                   len);
        }

        file.close();
    }

    void TFTPClient::close_socket_architecture() const
    {
        if (this->m_client_socket != INVALID_SOCKET)
        {
            closesocket(this->m_client_socket);
        }

        WSACleanup();

        std::cout << "Windows Socket Architecture is closed." << std::endl;
    }

    void TFTPClient::send_transmission_done_signal(const SOCKADDR* peer_addr)
    {
        sendto(this->m_client_socket,
               nullptr,
               sizeof(nullptr),
               0,
               peer_addr,
               sizeof(SOCKADDR)); //tell the server the transmission is done.

    }

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Protected Functions
 ******************************************************************************/


} // YB

/* End of File */
