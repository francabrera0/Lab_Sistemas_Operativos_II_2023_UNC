# Operating Systems II - LabIV

cmake_minimum_required(VERSION 3.22)

project(
    HPC_PARALLEL_PROGRAMMING
    VERSION 1.0.0
    DESCRIPTION "Edge filter algorithm"
    LANGUAGES C
)

set(CMAKE_C_STANDAR 11)

add_compile_options(-Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11 -O3 -fopenmp)

add_executable(Filter src/main.c src/gdalUtils.c src/edgeFilter.c)
target_include_directories(Filter PRIVATE "include/")
find_package(OpenMP REQUIRED)
target_link_libraries(Filter PRIVATE OpenMP::OpenMP_C)
target_link_libraries(Filter PRIVATE gdal)