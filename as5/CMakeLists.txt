cmake_minimum_required(VERSION 3.18)
project(as5 LANGUAGES CXX)
add_subdirectory(raylib-cpp)
file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as5 ${sources})
target_link_libraries(as5 PUBLIC raylib raygui raylib_cpp raylib::buffered)
set_property(TARGET as5 PROPERTY CXX_STANDARD 20)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/audio DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(includeable.cmake)