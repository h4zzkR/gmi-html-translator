cmake_minimum_required(VERSION 3.22)
project(html-generator)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -g -fsanitize-address-use-after-scope")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -fsanitize-address-use-after-scope")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g -fsanitize-address-use-after-scope")

add_subdirectory(library)

add_executable(html_gen main.cpp generator/generator.h generator/support/copy_batch.hpp generator/support/parts.hpp generator/fs_writer.hpp generator/translator.hpp)
target_link_libraries(html_gen exe)