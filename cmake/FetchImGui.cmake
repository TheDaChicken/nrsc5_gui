FetchContent_Declare(
        imgui_external
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.1
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

add_library(imgui STATIC
        ${imgui_external_SOURCE_DIR}/imgui.cpp
        ${imgui_external_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_external_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_external_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)