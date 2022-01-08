cmake_minimum_required(VERSION 3.0.0)
project(snake VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

add_executable(snake src/main.cpp src/Food.cpp src/Snake.cpp)
target_include_directories(snake PRIVATE ${CMAKE_SOURCE_DIR}/include/ ${CMAKE_SOURCE_DIR}/animation/include/)
target_link_libraries(snake -lsfml-graphics -lsfml-window -lsfml-system)