cmake_minimum_required(VERSION 3.10)
project(Technera)

set(CMAKE_CXX_STANDARD 17)

# External libraries
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/external/glfw")
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/external/glad")
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/external/imgui")

include_directories(
    ${GLFW_DIR}/include
    ${GLAD_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

link_directories(${GLFW_DIR}/lib-mingw-w64)

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(app
    src/main.cpp
    ${GLAD_DIR}/src/glad.c
    ${IMGUI_SOURCES}
)

target_link_libraries(app
    glfw3
    opengl32
    gdi32
)
