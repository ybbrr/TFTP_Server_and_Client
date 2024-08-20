///
/// @file tftp.hpp
/// @author Yasin BASAR
/// @brief Header file for TFTP operations including packet creation and block number management.
/// @version 1.0.0
/// @date 11/08/2024
/// @copyright (c) 2024 All rights reserved.
///

#ifndef TFTP_SEVER_AND_CLIENT_TFTP_HPP
#define TFTP_SEVER_AND_CLIENT_TFTP_HPP

////////////////////////////////////////////////////////////////////////////////
// Project Includes
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "types_enums_macros.hpp"

////////////////////////////////////////////////////////////////////////////////
// Third Party Includes
////////////////////////////////////////////////////////////////////////////////

namespace YB
{
    /// @class TFTP
    /// @brief Provides functions to create TFTP packets and manage block numbers.
    class TFTP
    {
    public:
    ////////////////////////////////////////////////////////////////////////////
    // Special Members
    ////////////////////////////////////////////////////////////////////////////

        TFTP() noexcept = delete; ///< Deleted default constructor
        ~TFTP() noexcept = delete; ///< Deleted default destructor
        TFTP(TFTP &&) noexcept = delete; ///< Deleted move constructor
        TFTP &operator=(TFTP &&) noexcept = delete;///< Deleted move assignment operator
        TFTP(const TFTP &) noexcept = delete;///< Deleted copy constructor
        TFTP &operator=(TFTP const &) noexcept = delete; ///< Deleted copy assignment operator

    ////////////////////////////////////////////////////////////////////////////
    // Public Members
    ////////////////////////////////////////////////////////////////////////////

        /// @brief Creates a Read Request (RRQ) packet.
        /// @param file_name The name of the file to be read.
        /// @return The RRQ packet.
        static packet_t make_rrq_packet(const std::string& file_name);

        /// @brief Creates a Write Request (WRQ) packet.
        /// @param file_name The name of the file to be written.
        /// @return The WRQ packet.
        static packet_t make_wrq_packet(const std::string& file_name);

        /// @brief Creates a Data packet.
        /// @param data_block A pointer to the data block.
        /// @return The Data packet.
        static packet_t make_data_packet(const char* data_block);

        /// @brief Creates an Acknowledgment (ACK) packet.
        /// @return The ACK packet.
        static packet_t make_ack_packet();

        /// @brief Creates an Error packet.
        /// @return The Error packet.
        static packet_t make_error_packet();

        /// @brief Resets the acknowledgment block number to its initial value.
        static void reset_ack_data_block_num();

        /// @brief Resets the data block number to its initial value.
        static void reset_data_block_num();

    /////////////////////////////////////////////////////////////////////////////
    // Private Members
    ////////////////////////////////////////////////////////////////////////////
    private:

        static int m_data_block_num; ///< Current data block number.
        static int m_ack_block_num; ///< Current acknowledgment block number.

    ////////////////////////////////////////////////////////////////////////////
    // Protected Members
    ////////////////////////////////////////////////////////////////////////////
    protected:

        // Data
    };

} // YB

#endif //TFTP_SEVER_AND_CLIENT_TFTP_HPP

/* End of File */
