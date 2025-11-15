cmake_minimum_required(VERSION 3.10)
project(Technera)

set(CMAKE_CXX_STANDARD 17)

# External libraries (relative paths)
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/external/glad")

include_directories(
    ${GLFW_DIR}/include
    ${GLAD_DIR}/include
)

link_directories(${GLFW_DIR}/lib-mingw-w64)

add_executable(app
    src/main.cpp
    ${GLAD_DIR}/src/glad.c
)

target_link_libraries(app
    glfw3
    opengl32
    gdi32
)
