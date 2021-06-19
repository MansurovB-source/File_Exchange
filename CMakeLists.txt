cmake_minimum_required(VERSION 3.19)
project(File_Exchange C)

set(CMAKE_C_STANDARD 11)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(File_Exchange
        main.c
        read_file.c
        read_file.h
        list.h
        list.c
        file_triplet.h
        file_triplet.c
        command_handler.c
        command_handler.h
        udp_search.c
        udp_search.h
        udp_server.c
        udp_server.h
        context.h
        tcp_client.c
        tcp_client.h
        tcp_server.c
        tcp_server.h
        events.c
        events.h ui_module.c ui_module.h)
target_link_libraries(File_Exchange OpenSSL::SSL Threads::Threads ${CURSES_LIBRARIES})