cmake_minimum_required(VERSION 3.16)

project(es-scenario-editor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia LinguistTools)
add_subdirectory(external/QScintilla)
set(TS_FILES es-scenario-editor_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        menubar.cpp
        menubar.h
        menuactions.h
        docsdialog.cpp
        docsdialog.h
        assetpreview.cpp
        assetpreview.h
        resources.qrc
        editor/codeeditor.h
        editor/codeeditor.cpp
        editor/renpylexer.h
        editor/renpylexer.cpp
        projectcreatedialog.h
        projectcreatedialog.cpp
        projectcreator.h
        projectcreator.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(es-scenario-editor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        assetpreview.h
        assetpreview.cpp
        editor/codeeditor.h
        editor/codeeditor.cpp
        editor/renpylexer.h
        editor/renpylexer.cpp
        projectcreatedialog.h
        projectcreator.h
        projectcreator.cpp
        projectcreatedialog.cpp
        newfiledialog.h
        themes/thememanager.h
        themes/themesmanager.cpp
        settings/settingsdialog.h
        settings/settingsdialog.cpp
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(es-scenario-editor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(es-scenario-editor
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(es-scenario-editor PRIVATE
    src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/QScintilla/src
)

target_link_libraries(es-scenario-editor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    $<$<CONFIG:Debug>:qscintilla2_qt6d>
    $<$<CONFIG:Release>:qscintilla2_qt6>
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.es-scenario-editor)
endif()
set_target_properties(es-scenario-editor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS es-scenario-editor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(es-scenario-editor)
endif()
