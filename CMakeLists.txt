cmake_minimum_required(VERSION 3.25)

project(TFTP_Sever_and_Client)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(TFTP)
add_subdirectory(TFTP_Client)
add_subdirectory(TFTP_Server)

# end of file
