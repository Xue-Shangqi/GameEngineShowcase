cmake_minimum_required(VERSION 3.18)
project(as4 LANGUAGES CXX)
add_subdirectory(raylib-cpp)
file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as4 ${sources})
target_link_libraries(as4 PUBLIC raylib raylib_cpp)
set_property(TARGET as4 PROPERTY CXX_STANDARD 20)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/customModel DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sound DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(includeable.cmake)

# make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.vs)
# make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.fs)
# make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.vs)
# make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.fs)