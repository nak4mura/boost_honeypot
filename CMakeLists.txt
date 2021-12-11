cmake_minimum_required(VERSION 3.16)
project(boost_log)

find_package(Boost REQUIRED COMPONENTS system filesystem thread program_options log)
find_package(Threads REQUIRED)

add_definitions("-Wall --pedantic-errors")
add_definitions(-DBOOST_LOG_DYN_LINK)

include_directories(Boost::boost)

message("## Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")  # デバッグ用：解決したパスを表示
message("## Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")  # デバッグ用：解決したパスを表示

set(OBJS boost_honeypot.cpp http_request.cpp http_response.cpp logger.cpp tcp_connection.cpp tcp_server.cpp read_response_conf.cpp read_server_conf.cpp)

add_executable(boosthoney ${OBJS})
target_link_libraries(boosthoney Boost::system Boost::filesystem Boost::thread Boost::program_options Boost::log Threads::Threads)
set(TARGETS boosthoney)

set_property(TARGET ${TARGETS} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${TARGETS} PROPERTY CXX_STANDARD_REQUIRED 17)
set_property(TARGET ${TARGETS} PROPERTY CXX_EXTENSIONS 17)
