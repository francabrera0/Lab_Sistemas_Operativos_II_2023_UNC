# Operating Systems II - LabII

cmake_minimum_required(VERSION 3.22)

project(
    IPCAdvanced
    VERSION 1.0.0
    DESCRIPTION "Client Server architecture with linux IPC"
    LANGUAGES C
)

set(CMAKE_C_STANDAR 11)

add_compile_options(-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11)

add_executable(Server src/server/server.c src/server/ipv4Server.c src/server/ipv6Server.c src/server/unixServer.c src/server/serverUtils.c)
target_include_directories(Server PRIVATE "include/server/")
target_include_directories(Server PRIVATE "include/utils/")
target_link_libraries(Server PRIVATE cjson)

add_executable(Client src/client/client.c src/client/ipv4Client.c src/client/ipv6Client.c src/client/unixClient.c src/client/clientUtils.c)
target_include_directories(Client PRIVATE "include/client/")
target_include_directories(Client PRIVATE "include/utils/")
target_link_libraries(Client PRIVATE cjson)