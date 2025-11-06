cmake_minimum_required(VERSION 3.16)
project(COP3530-Project2)

set(CMAKE_CXX_STANDARD 17) # using c++17 for this project

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DASIO_STANDALONE) # use asio standalone

add_executable(COP3530-Project2 main.cpp
        crow_all.h
        parse.h
        weightedTrie.h
        datahandler.h
        minHeap.h)



set(CPR_USE_SYSTEM_CURL ON) # IMPORTANT: YOU MAY NEED TO HOMEBREW/INSTALL CURL ON YOUR SYSTEM, DO NOT CHANGE

include(FetchContent)
# init cpr library
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 6e925d0fcb74377a4aa108eff4edfb6b0aab4750)
FetchContent_MakeAvailable(cpr)

#init asio library (dependency for Crow)
FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-1
)
FetchContent_MakeAvailable(asio)


target_link_libraries(COP3530-Project2 PRIVATE cpr::cpr)
target_include_directories(COP3530-Project2 PRIVATE ${asio_SOURCE_DIR}/asio/include)