cmake_minimum_required(VERSION 3.19)
project(nbtp3)

find_library(CURL_LIBRARY curl)
find_library(PROTOBUF_LIBRARY protobuf)
set(source_code_list
        src/converter.cpp
        src/Dispatcher.cpp
        src/downloader.cpp
        src/nbtp.cpp
        src/engine/engine.cpp
        src/engine/localEngine.cpp
        src/lodepng/lodepng.cpp
        src/communicate/reporter.cpp
        src/communicate/socket_server/WindowsSocketServer.cpp
        src/communicate/nbtp_protocol.pb.cc
        )
#add_subdirectory(Google_tests)
set(CMAKE_CXX_STANDARD 20)
include_directories(./src src/communicate/socket_server src/communicate)
add_executable(nbtp3 ${source_code_list} src/main.cpp)
add_executable(test ${source_code_list} test/test_SServer.cpp)
target_link_libraries(nbtp3 ${CURL_LIBRARY} ${PROTOBUF_LIBRARY})
target_link_libraries(test ${CURL_LIBRARY} ${PROTOBUF_LIBRARY})
if(WIN32)
    target_link_libraries(test wsock32 ws2_32)
endif()