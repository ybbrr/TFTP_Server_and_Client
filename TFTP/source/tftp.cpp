/**
 * @file tftp.cpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */


/*******************************************************************************
 * Includes 
 ******************************************************************************/

#include "tftp.hpp"
#include <WinSock2.h>
#include <string>

/*******************************************************************************
 * Third Party Libraries 
 ******************************************************************************/


namespace YB
{
    typedef struct TFTP_header_s
    {
        uint16_t op_code;
    } TFTP_header_t;

    typedef struct TFTP_data_block_s
    {
        uint16_t data_block;
    } TFTP_data_block_t;

    int TFTP::m_data_block_num = 1U;
    int TFTP::m_ack_block_num = 1U;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

    packet_t TFTP::make_rrq_packet(const std::string& file_name)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_RRQ);
        int file_name_len = file_name.length() + 1;
        int header_size = sizeof(header);
        int data_len = header_size + file_name_len + 5 + 1;
        packet_t rrq;
        rrq.data_ptr = std::make_unique<char[]>(data_len);
        rrq.size = data_len;
        rrq.data_block_number = -1;
        memcpy(rrq.data_ptr.get(), &header, header_size);
        memcpy(rrq.data_ptr.get() + header_size, file_name.c_str(), file_name_len);
        memcpy(rrq.data_ptr.get() + header_size + file_name_len, "octet", 6);
        return rrq;
    }

    packet_t TFTP::make_wrq_packet(const std::string& file_name)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_WRQ);
        int file_name_len = file_name.length() + 1;
        int header_size = sizeof(header);
        int data_len = header_size + file_name_len + 5 + 1;
        packet_t wrq;
        wrq.data_ptr = std::make_unique<char[]>(data_len);
        wrq.size = data_len;
        wrq.data_block_number = -1;
        memcpy(wrq.data_ptr.get(), &header, header_size);
        memcpy(wrq.data_ptr.get() + header_size, file_name.c_str(), file_name_len);
        memcpy(wrq.data_ptr.get() + header_size + file_name_len, "octet", 6);
        return wrq;
    }

    packet_t TFTP::make_data_packet(const char* data_block)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_DATA);
        TFTP_data_block_t block{};
        block.data_block = htons(m_data_block_num);
        int header_size = sizeof(header);
        int block_size = sizeof(block);
        int data_len = header_size + block_size + TFTP_OUTGOING_DATA_BUFFER_LEN;
        packet_t data_packet;
        data_packet.data_ptr = std::make_unique<char[]>(data_len);
        data_packet.size = data_len;
        data_packet.data_block_number = m_data_block_num++;
        memcpy(data_packet.data_ptr.get(), &header, sizeof(header));
        memcpy(data_packet.data_ptr.get() + header_size, &block, block_size);
        memcpy(data_packet.data_ptr.get() + header_size + block_size, data_block, TFTP_OUTGOING_DATA_BUFFER_LEN);
        return data_packet;
    }

    packet_t TFTP::make_ack_packet()
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_ACK);
        TFTP_data_block_t block{};
        block.data_block = htons(m_ack_block_num);
        int header_size = sizeof(header);
        int block_size = sizeof(block);
        int data_len = header_size + block_size + 1;
        packet_t ack_packet;
        ack_packet.data_ptr = std::make_unique<char[]>(data_len);
        ack_packet.size = data_len;
        ack_packet.data_block_number = m_ack_block_num++;
        memcpy(ack_packet.data_ptr.get(), &header, header_size);
        memcpy(ack_packet.data_ptr.get() + header_size, &block, block_size + 1);
        return ack_packet;
    }

    packet_t TFTP::make_error_packet()
    {
        std::string err_msg = "Something went wrong between server and client.";

        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_ERR);
        TFTP_data_block_t block{};
        block.data_block = htons(m_ack_block_num);
        int header_size = sizeof(header);
        int block_size = sizeof(block);
        int data_len = header_size + block_size + err_msg.length() + 1;
        packet_t error_packet;
        error_packet.data_ptr = std::make_unique<char[]>(data_len);
        error_packet.size = data_len;
        error_packet.data_block_number = m_ack_block_num++;
        memcpy(error_packet.data_ptr.get(), &header, header_size);
        memcpy(error_packet.data_ptr.get() + header_size, &block, block_size);
        memcpy(error_packet.data_ptr.get() + header_size + block_size, err_msg.c_str(), err_msg.length());
        return error_packet;
    }

    void TFTP::reset_ack_data_block_num()
    {
        m_ack_block_num = 1U;
    }

    void TFTP::reset_data_block_num()
    {
        m_data_block_num = 1U;
    }

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Protected Functions
 ******************************************************************************/

} // YB

/* End of File */
