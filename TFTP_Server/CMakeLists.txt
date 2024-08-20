cmake_minimum_required(VERSION 3.25)

project(TFTP_Sever)

set(CMAKE_CXX_STANDARD 17)

if (EDITOR_BUILD)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
	set(CMAKE_INSTALL_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	if (MSVC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /O2 /MD")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /O2 /MD /arch:AVX2")
	endif ()

	if (UNIX)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -march=native")
	endif ()
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	if (MSVC)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /Od /MDd")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /Od /MDd /arch:AVX2")
	endif ()

	if (UNIX)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -g -Wall -ggdb")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -Wall -ggdb -march=native")
	endif ()
endif ()

set(WORKSPACE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(BASE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
	add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS)
	set(WINSOCK_LIB Ws2_32)
endif ()

# Project Includes
include_directories(${BASE_FOLDER}/include)

# Third Party Includes
include_directories(${WORKSPACE_FOLDER}/TFTP/include)
include_directories(${WORKSPACE_FOLDER}/TFTP/util)

add_executable(
	${PROJECT_NAME}

	${BASE_FOLDER}/main.cpp
	${BASE_FOLDER}/source/tftp_server.cpp)

target_link_libraries(
	${PROJECT_NAME}

	PRIVATE

	TFTP
	${WINSOCK_LIB})

install(TARGETS ${PROJECT_NAME}
		DESTINATION ${CMAKE_INSTALL_PREFIX})

# end of file
