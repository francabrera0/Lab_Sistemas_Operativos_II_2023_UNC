# Operating Systems II - LabII

cmake_minimum_required(VERSION 3.22)

project(
    IPCAdvanced
    VERSION 1.0.0
    DESCRIPTION "API REST"
    LANGUAGES C
)

set(CMAKE_C_STANDAR 11)

add_compile_options(-Wall -Werror -pedantic  -Wextra -Wconversion -std=gnu11)

add_executable(Server src/server/server.c )
target_include_directories(Server PRIVATE "include/server/")
target_link_libraries(Server PRIVATE ulfius)
target_link_libraries(Server PRIVATE cjson)

add_executable(Client src/client/client.c )
target_link_libraries(Client PRIVATE curl)
