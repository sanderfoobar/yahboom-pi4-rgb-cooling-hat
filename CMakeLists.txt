cmake_minimum_required(VERSION 3.15)
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")
project(rgb_cooling_hat C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)

message(STATUS "")

add_executable(rgb_cooling_hat main.c lib/ssd1306_i2c.c lib/ssd1306_i2c.h lib/oled_fonts.h)

target_link_libraries(rgb_cooling_hat PRIVATE -lwiringPi)
target_include_directories(rgb_cooling_hat PRIVATE lib)

install(TARGETS rgb_cooling_hat)
