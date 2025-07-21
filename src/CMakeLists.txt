add_executable(${PROJECT_NAME}
        gui/backends/GlfwImpl.cpp
        gui/imgui/Imgui.cpp
        Application.cpp
        main.cpp
        gui/widgets/ButtonGroup.cpp
        gui/widgets/ImageButton.cpp
        gui/TextureAsset.cpp
        gui/ImageManager.cpp
        gui/image/PNGDecoder.cpp
        gui/image/PNGDecoder.h
        gui/image/Image.h
        gui/image/SVGDecoder.cpp
        utils/Log.h
        utils/Log.cpp
        gui/AssetsManager.cpp
        gui/AssetsManager.h
)

set(OUTPUT_RESOURCE_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/resources/fonts/OpenSans-VariableFont.ttf
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/BlueBack.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Heart.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Minus.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Play.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Pause.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Plus.svg
        ${CMAKE_CURRENT_BINARY_DIR}/resources/buttons/black/Navigation.svg
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources/
        ${CMAKE_CURRENT_BINARY_DIR}/resources)

target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        imgui
        PNG::PNG
        lunasvg::lunasvg
        fmt::fmt
        spdlog::spdlog
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
        RESOURCE "${OUTPUT_RESOURCE_FILES}"
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RESOURCE DESTINATION ${PROJECT_RESOURCE_DIR}
)