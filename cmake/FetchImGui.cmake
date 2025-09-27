FetchContent_Declare(
        imgui_external
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.3
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

add_library(imgui STATIC
        ${imgui_external_SOURCE_DIR}/imgui.cpp
        ${imgui_external_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_external_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_external_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
        ${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
        ${imgui_external_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)
target_compile_definitions(imgui PUBLIC
        IMGUI_ENABLE_FREETYPE=ON
        IMGUI_USE_WCHAR32=ON
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS=ON
        # IMGUI_USER_CONFIG="${imgui_external_SOURCE_DIR}/imgui_user_config.h"
)
target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})
target_link_libraries(imgui PUBLIC SDL3::SDL3 Freetype::Freetype)