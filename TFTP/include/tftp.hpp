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

        virtual ~TFTP() noexcept = default;

        TFTP(TFTP &&) noexcept = default;

        TFTP &operator=(TFTP &&) noexcept = default;

        TFTP(const TFTP &) noexcept = default;

        TFTP &operator=(TFTP const &) noexcept = default;

    /***************************************************************************
     * Public Members
     **************************************************************************/

        TFTP();

        std::vector<char> make_rrq_packet(const std::string& file_name);

        std::vector<char> make_wrq_packet(const std::string& file_name);

        std::vector<char> make_data_packet(const char* data_block);

        std::vector<char> make_ack_packet();

        std::vector<char> make_error_packet();

        void reset_ack_data_block_num();

        void reset_data_block_num();

    /***************************************************************************
     * Private Members
     **************************************************************************/
    private:

        size_t m_data_block_num; // use only for makeDataPacket
        size_t m_ack_data_block_num; // use only for makeACKPacket

    /***************************************************************************
     * Protected Members
     **************************************************************************/
    protected:

        /* Data */
    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_HPP

/* End of File */
