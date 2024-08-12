/**
 * @file tftp.hpp
 * @author Yasin BASAR
 * @brief
 * @version 1.0.0
 * @date 11/08/2024
 * @copyright (c) 2024 All rights reserved.
 */

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_HPP

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <string>
#include <vector>
#include "types_enums_macros.hpp"

/*******************************************************************************
 * Third Party Libraries
 ******************************************************************************/

namespace YB
{

    class TFTP
    {
    public:
    /***************************************************************************
     * Special Members
     **************************************************************************/

        TFTP() noexcept = delete;

        virtual ~TFTP() noexcept = delete;

        TFTP(TFTP &&) noexcept = delete;

        TFTP &operator=(TFTP &&) noexcept = delete;

        TFTP(const TFTP &) noexcept = delete;

        TFTP &operator=(TFTP const &) noexcept = delete;

    /***************************************************************************
     * Public Members
     **************************************************************************/

        static packet_t make_rrq_packet(const std::string& file_name);

        static packet_t make_wrq_packet(const std::string& file_name);

        static packet_t make_data_packet(const char* data_block);

        static packet_t make_ack_packet();

        static packet_t make_error_packet();

        static void reset_ack_data_block_num();

        static void reset_data_block_num();

    /***************************************************************************
     * Private Members
     **************************************************************************/
    private:

        static int m_data_block_num;
        static int m_ack_block_num;

    /***************************************************************************
     * Protected Members
     **************************************************************************/
    protected:

        /* Data */
    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_HPP

/* End of File */
