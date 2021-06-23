CC=gcc
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=  main.c utils/dir_reader/read_file.c utils/dir_reader/read_file.h utils/list/list.h utils/list/list.c utils/command_handler.c utils/command_handler.h network/udp_search.c network/udp_search.h network/udp_server.c network/udp_server.h utils/context.h network/tcp_client.c network/tcp_client.h network/tcp_server.c network/tcp_server.h event/events.c event/events.h ui/ui_module.c ui/ui_module.h utils/util.c utils/util.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mainn

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@