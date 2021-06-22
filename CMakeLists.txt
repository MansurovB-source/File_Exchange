cmake_minimum_required(VERSION 3.19)
project(File_Exchange C)

set(CMAKE_C_STANDARD 11)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(File_Exchange
        main.c
        utils/dir_reader/read_file.c
        utils/dir_reader/read_file.h
        utils/list/list.h
        utils/list/list.c
        utils/command_handler.c
        utils/command_handler.h
        network/udp_search.c
        network/udp_search.h
        network/udp_server.c
        network/udp_server.h
        utils/context.h
        network/tcp_client.c
        network/tcp_client.h
        network/tcp_server.c
        network/tcp_server.h
        event/events.c
        event/events.h
        ui/ui_module.c
        ui/ui_module.h
        utils/util.c
        utils/util.h)
target_link_libraries(File_Exchange OpenSSL::SSL Threads::Threads ${CURSES_LIBRARIES})