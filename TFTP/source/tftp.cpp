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

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

    TFTP::TFTP()
        : m_data_block_num{1U},
          m_ack_data_block_num{1U}
    {
    }

    std::vector<char> TFTP::make_rrq_packet(const std::string& file_name)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_RRQ);
        size_t file_name_len = file_name.length() + 1;
        size_t header_size = sizeof(header);
        size_t data_len = header_size + file_name_len + 5 + 1;
        std::vector<char> rrq(data_len, 0U);
        std::memcpy(rrq.data(), &header, header_size);
        std::memcpy(rrq.data() + header_size, file_name.c_str(), file_name_len);
        std::memcpy(rrq.data() + header_size + file_name_len, "octet", 6);
        return rrq;
    }

    std::vector<char> TFTP::make_wrq_packet(const std::string& file_name)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_WRQ);
        size_t file_name_len = file_name.length() + 1;
        size_t header_size = sizeof(header);
        size_t data_len = header_size + file_name_len + 5 + 1;
        std::vector<char> wrq(data_len, 0U);
        std::memcpy(wrq.data(), &header, header_size);
        std::memcpy(wrq.data() + header_size, file_name.c_str(), file_name_len);
        std::memcpy(wrq.data() + header_size + file_name_len, "octet", 6);
        return wrq;
    }

    std::vector<char> TFTP::make_data_packet(const char* data_block)
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_DATA);
        TFTP_data_block_t block{};
        block.data_block = htons(this->m_data_block_num++);
        size_t header_size = sizeof(header);
        size_t block_size = sizeof(block);
        size_t data_len = header_size + block_size + TFTP_OUTGOING_DATA_BUFFER_LEN;
        std::vector<char> data_packet(data_len, 0U);
        std::memcpy(data_packet.data(), &header, sizeof(header));
        std::memcpy(data_packet.data() + header_size, &block, block_size);
        std::memcpy(data_packet.data() + header_size + block_size, data_block, TFTP_OUTGOING_DATA_BUFFER_LEN);
        return data_packet;
    }

    std::vector<char> TFTP::make_ack_packet()
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_ACK);
        TFTP_data_block_t block{};
        block.data_block = htons(this->m_ack_data_block_num++);
        size_t header_size = sizeof(header);
        size_t block_size = sizeof(block);
        size_t data_len = header_size + block_size + 1;
        std::vector<char> ack_packet(data_len, 0U);
        std::memcpy(ack_packet.data(), &header, header_size);
        std::memcpy(ack_packet.data() + header_size, &block, block_size + 1);
        return ack_packet;
    }

    std::vector<char> TFTP::make_error_packet()
    {
        TFTP_header_t header{};
        header.op_code = htons(OP_CODE_ERR);
        TFTP_data_block_t block{};
        block.data_block = htons(this->m_ack_data_block_num++);
        std::vector<char> error_packet(sizeof(header) + sizeof(block), 0U);
        std::memcpy(error_packet.data(), &header, sizeof(header));
        std::memcpy(error_packet.data() + sizeof(header), &block, sizeof(block));

        std::string err_msg = "Something went wrong between server and client.";
        std::memcpy(error_packet.data() + sizeof(header) + sizeof(block), err_msg.c_str(), err_msg.length() + 2);
        return error_packet;
    }

    void TFTP::reset_ack_data_block_num()
    {
        this->m_ack_data_block_num = 1U;
    }

    void TFTP::reset_data_block_num()
    {
        this->m_data_block_num = 1U;
    }

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Protected Functions
 ******************************************************************************/

} // YB

/* End of File */
