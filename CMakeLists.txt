cmake_minimum_required(VERSION 3.10)
project(Laba2_OS VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_library(gauss STATIC
    src/gauss.c
)

add_library(threads_lim STATIC
    src/threads_lim.c
)

add_executable(Laba2_OS src/main.c)

target_link_libraries(Laba2_OS gauss threads_lim)

find_package(Threads REQUIRED)
target_link_libraries(Laba2_OS Threads::Threads)
target_link_libraries(gauss Threads::Threads)
target_link_libraries(threads_lim Threads::Threads)

set_target_properties(Laba2_OS gauss threads_lim
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)