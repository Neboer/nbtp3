cmake_minimum_required(VERSION 3.19)
project(nbtp3)

set(GOOGLETEST_VERSION 1.11.0)
find_library(CURL_LIBRARY curl)
set(source_code_list
        src/TQueue/TQueue.cpp
        src/converter.cpp
        src/Dispatcher.cpp
        src/downloader.cpp
        src/nbtp.cpp
        src/reporter.cpp
        src/engine/engine.cpp
        src/engine/localEngine.cpp
        src/lodepng/lodepng.cpp
        )
#add_subdirectory(Google_tests)
set(CMAKE_CXX_STANDARD 20)
add_executable(nbtp3 ${source_code_list} src/main.cpp)
target_link_libraries(nbtp3 ${CURL_LIBRARY})