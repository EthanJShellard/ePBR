cmake_minimum_required(VERSION 3.0)
project(ePBR)
# Needed to compile and link glew statically
add_definitions(-DGLEW_STATIC)

add_library(glew
    src/GL/glew.h
    src/GL/glew.c
    src/GL/wglew.h
)

add_library(imgui
    src/imgui/imconfig.h
    src/imgui/imgui.cpp
    src/imgui/imgui.h
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_impl_opengl3.cpp
    src/imgui/imgui_impl_opengl3.h
    src/imgui/imgui_impl_opengl3_loader.h
    src/imgui/imgui_impl_sdl.cpp
    src/imgui/imgui_impl_sdl.h
    src/imgui/imgui_impl_sdlrenderer.cpp
    src/imgui/imgui_impl_sdlrenderer.h
    src/imgui/imgui_internal.h
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imstb_rectpack.h
    src/imgui/imstb_textedit.h
    src/imgui/imstb_truetype.h
)

add_executable(ePBR
    src/ePBR/GameObject.h
    src/ePBR/GameObject.cpp
    src/ePBR/Main.cpp
    src/ePBR/Material.h
    src/ePBR/Material.cpp
    src/ePBR/Mesh.h
    src/ePBR/Mesh.cpp
    src/ePBR/Scene.h
    src/ePBR/Scene.cpp
)

target_include_directories(ePBR
    PUBLIC include/common
    PUBLIC src/ # For glew and imgui
)

target_include_directories(imgui
    PUBLIC include/common
    PUBLIC src/ # For glew
)

target_link_libraries(ePBR
    ${PROJECT_SOURCE_DIR}/lib/Windows-x64/SDL2.lib
    ${PROJECT_SOURCE_DIR}/lib/Windows-x64/SDL2main.lib
    opengl32
    imgui
    glew
)