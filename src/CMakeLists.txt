# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g")

add_library(${PROJECT_NAME}-lib STATIC
        gui/widgets/IconButton.cpp
        gui/wrappers/Texture.cpp
        gui/image_decoders/ImageDecoder.cpp
        gui/image_decoders/PNGDecoder.cpp
        gui/image_decoders/PNGDecoder.h
        gui/image_decoders/ImageBuffer.h
        gui/image_decoders/SVGDecoder.cpp
        gui/managers/AssetsManager.cpp
        gui/managers/AssetsManager.h
        gui/image_decoders/JPEGDecoder.cpp
        gui/image_decoders/JPEGDecoder.h
        gui/widgets/Navigation.cpp
        gui/widgets/Navigation.h
        gui/widgets/DualIconButton.cpp
        gui/widgets/DualIconButton.h
        gui/panels/DockStationPanel.cpp
        gui/panels/DockStationPanel.h
        gui/managers/ThemeManager.cpp
        gui/managers/ThemeManager.h
        gui/widgets/ListView.cpp
        gui/widgets/ListView.h
        gui/view/radio/RadioView.cpp
        gui/view/radio/RadioView.h
        gui/panels/DockFavoritesPanel.cpp
        gui/panels/DockFavoritesPanel.h
        gui/panels/DockAudioPanel.cpp
        gui/panels/DockAudioPanel.h
        gui/panels/DockTunePanel.cpp
        gui/panels/DockTunePanel.h
        gui/view/SettingsView.cpp
        gui/view/SettingsView.h
        gui/utils/Icon.h
        gui/utils/Cache.h
        gui/panels/DockInputPanel.cpp
        gui/panels/DockInputPanel.h
        gui/managers/EventsDispatcher.cpp
        gui/managers/EventsDispatcher.h

        utils/Log.h
        utils/Log.cpp

        sql/connection/Connection.cpp
        sql/connection/ConnectionPool.cpp
        sql/sqlite/sqlite3.c

        nrsc5/LotManager.cpp
        nrsc5/Station.cpp
        nrsc5/Processor.cpp

        dsp/Firdes.cpp
        dsp/Window.cpp
        dsp/resampler/ArbResampler.h

        hybrid/external/HybridExternal.cpp
        hybrid/external/HybridExternal.h
        hybrid/HybridSessionManager.cpp
        hybrid/HybridSessionManager.h
        hybrid/HybridController.cpp
        hybrid/HybridController.h

        models/FavoriteList.cpp
        models/FavoriteList.h

        utils/ThreadPool.cpp
        utils/ThreadPool.h
        utils/RingBuffer.cpp
        utils/RingBuffer.h
        utils/Expected.hpp

        gui/managers/ImageManager.cpp
        gui/managers/ImageManager.h
        sql/DatabaseManager.cpp
        sql/DatabaseManager.h
        input/SDRInput.cpp
        input/SDRInput.h
        input/RadioInput.h
        hybrid/HybridInput.cpp
        hybrid/HybridInput.h
        sql/SchemaTable.cpp
        sql/SchemaTable.h
        sql/SettingsTable.cpp
        sql/SettingsTable.h
        sql/LotTable.cpp
        sql/LotTable.h

        hybrid/HybridState.h

        audio/AudioManager.cpp
        audio/AudioManager.h
        dsp/resampler/IFilterBlock.h
        dsp/resampler/IFilterStream.h

        gui/wrappers/TextureUploader.cpp
        gui/wrappers/TextureUploader.h
        gui/wrappers/GPUContext.cpp
        gui/wrappers/GPUContext.h
        hybrid/external/services/LotService.cpp
        hybrid/external/services/LotService.h
        hybrid/external/HybridServiceUtil.h
        hybrid/HybridSession.cpp
        hybrid/HybridSession.h

        gui/view/radio/RadioHomeView.cpp
        gui/view/radio/RadioHomeView.h
        gui/view/radio/RadioBackView.cpp
        gui/view/radio/RadioBackView.h
        gui/SDLWindow.cpp
        gui/SDLWindow.h
        gui/view/IView.h
)

target_link_libraries(${PROJECT_NAME}-lib PUBLIC
        SDL3::SDL3
        imgui
        PNG::PNG
        lunasvg::lunasvg
        JPEG::JPEG
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        nrsc5::nrsc5
        Freetype::Freetype
        PortSDR
        Volk::volk
)

add_executable(${PROJECT_NAME}
        main.cpp
        Application.cpp
        Application.h
        gui/MainWindow.cpp
        gui/MainWindow.h
        gui/Util.h
        gui/utils/TextRender.cpp
        gui/utils/TextRender.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}-lib
)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/resources/
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
)
add_dependencies(${PROJECT_NAME} copy_assets)

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