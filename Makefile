CC = g++
DEST = ./
GCC_OPTIONS = -std=c++17 -Wall --pedantic-errors
LDFLAGS = -L /usr/lib -no-pie
BOOST_OPTIONS = -lboost_system -lboost_filesystem -lboost_thread -lpthread -lboost_program_options -lboost_log -DBOOST_LOG_DYN_LINK
OPTIONS = $(GCC_OPTIONS) $(LDFLAGS) $(BOOST_OPTIONS)
OBJS = boost_honeypot.cpp http_request.cpp http_response.cpp logger.cpp tcp_connection.cpp tcp_server.cpp read_response_conf.cpp read_server_conf.cpp
HEADER = all.hpp
HEADERS = http_request.hpp http_response.hpp logger.hpp tcp_connection.hpp tcp_server.hpp read_response_conf.hpp read_server_conf.hpp

boosthoney : $(OBJS) all.hpp.gch
	$(CC) $(OPTIONS) $(OBJS) -o $@

all.hpp.gch : $(HEADERS)
	$(CC) $(GCC_OPTIONS) -x c++-header -o $@ $(HEADER)

run : boosthoney
	sudo ./boosthoney

clean :
	rm -f ./boosthoney
	rm -f ./all.hpp.gch

.PHONY : run clean
