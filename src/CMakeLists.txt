project(UI)

set(APPLICATION "nrsc5_gui")

qt_add_executable(${APPLICATION}
        windows/MainWindow.cpp
        windows/MainWindow.ui

        pages/LoadingPage.cpp
        pages/RadioPage.cpp
        pages/SettingsPage.cpp

        settings/TunerPage.cpp
        settings/AboutPage.cpp

        models/ChannelModel.cpp
        models/LinkedChannelModel.cpp
        models/TunerDevicesModel.cpp

        delegates/ChannelDelegate.cpp
        delegates/BasicTextDelegate.cpp

        controllers/StationInfoManager.cpp
        controllers/RadioController.cpp

        display/StationInfoPanel.cpp
        display/RadioInfoDisplay.cpp
        display/RadioInfoDisplay.ui

        widgets/dualview/DualViewContainer.h
        widgets/dualview/DualViewWidget.cpp
        widgets/dualview/DualViewLayout.cpp

        widgets/navigation/NavigationHeader.cpp
        widgets/navigation/NavigationWidget.cpp

        widgets/AnimatedStackedWidget.cpp
        widgets/StationFramePanel.cpp

        widgets/favorites/FavoritesList.cpp
        widgets/favorites/FavoritesHeader.cpp
        widgets/ListViewHeader.cpp
        widgets/LineHeader.cpp
        widgets/text/ImageText.cpp
        widgets/text/ImageTextLabel.cpp
        widgets/text/ImageTextEdit.cpp
        widgets/text/TextSlider.cpp
        widgets/progress/ProgressCircleText.cpp
        widgets/CapsLabelAlign.cpp
        widgets/RoundButton.cpp
        widgets/sdr/TuneWidget.cpp
        widgets/ClickableQLabel.cpp
        widgets/sdr/GainSettings.cpp

        views/RadioControlPanelView.cpp
        views/RadioMainView.cpp
        views/SettingsView.cpp

        themes/ThemeManager.cpp

        utils/StylingText.cpp

        Application.cpp
        Main.cpp

        ${CMAKE_SOURCE_DIR}/resources/Resources.qrc
)

target_link_libraries(${APPLICATION} PUBLIC
        nrsc5_lib
        PRIVATE
        Qt6::OpenGL
        Qt6::Svg
        Qt6::Concurrent
        Qt6::PrintSupport
)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif ()

set_target_properties(${APPLICATION} PROPERTIES
        AUTOUIC ON
        AUTORCC ON
        AUTOMOC ON
)

if (TARGET Qt6::QSQLiteDriverPlugin)
    target_link_libraries(${APPLICATION} PRIVATE Qt6::QSQLiteDriverPlugin)
endif ()

target_include_directories(${APPLICATION} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/autogen/
)
set_target_properties(${APPLICATION} PROPERTIES LINK_FLAGS "${LINKER_FLAGS}")
set_target_properties(${APPLICATION} PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)

install(TARGETS ${APPLICATION}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(APPLE)
    set(executable_path "$<TARGET_FILE_NAME:${APPLICATION}>.APPLICATION")
else()
    set(executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${APPLICATION}>")
endif()

set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_${APPLICATION}.cmake")

file(GENERATE OUTPUT ${deploy_script} CONTENT "
include(\"${QT_DEPLOY_SUPPORT}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${executable_path}\"
    PLUGINS_DIR bin
    NO_TRANSLATIONS
)"
)

install(SCRIPT ${deploy_script})