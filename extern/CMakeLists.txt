
cmake_minimum_required(VERSION 3.10)

set(NAME imgui)
project(${NAME})

set(CMAKE_CXX_STANDARD 17)

add_library(
        ${NAME} STATIC

        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/examples/imgui_impl_opengl3.cpp
        imgui/examples/imgui_impl_opengl3.h
        imgui/examples/imgui_impl_glfw.cpp
        imgui/examples/imgui_impl_glfw.h
        imgui/imgui_internal.h
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
)

include_directories( SYSTEM
        ${CMAKE_SOURCE_DIR}/extern/imgui/
        ${CMAKE_SOURCE_DIR}/extern/glad/include/
        ${CMAKE_SOURCE_DIR}/extern/imgui/examples/
        ${CMAKE_SOURCE_DIR}/extern/glfw/include/)

target_include_directories(
        ${NAME} PUBLIC
        $ENV{HOME}/.brew/include
)

target_compile_definitions(${NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD=1)