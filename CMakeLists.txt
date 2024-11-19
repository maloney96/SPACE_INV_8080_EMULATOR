cmake_minimum_required(VERSION 3.16)

project(SpaceInvadersEmulator VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools OpenGLWidgets)

set(TS_FILES SpaceInvadersEmulator_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        ${TS_FILES}
        images/Celestial.png images/spaceSky.jpg images/Wisdom.png
        ROM/invaders.e ROM/invaders.f ROM/invaders.g ROM/invaders.h
        resources.qrc

        inputmanager/inputManager.cpp inputmanager/inputManager.h inputmanager/keymap.h
        inputmanager/debugwrapper.cpp inputmanager/debugwrapper.h
        inputmanager/romassembler.cpp inputmanager/romassembler.h

        ui/instructions.cpp ui/instructions.h ui/instructions.ui
        ui/mainwindow.cpp ui/mainwindow.h ui/mainwindow.ui
        ui/settings.h ui/settings.cpp ui/settings.ui
        ui/setkeydialog.h ui/setkeydialog.cpp ui/setkeydialog.ui
        ui/glwidget.h ui/glwidget.cpp

        outputmanager/outputManager.cpp outputmanager/outputManager.h
        outputmanager/videoemulator.c outputmanager/videoemulator.h

        disassembler/disassembler.c disassembler/disassembler.h
        emulator/emulator.c emulator/emulator.h
        emulator/emulatorWrapper.cpp emulator/emulatorWrapper.h
        emulator/io_bits.h emulator/ioports_t.h
        memory/mem_utils.c memory/mem_utils.h
        memory/memory.c memory/memory.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SpaceInvadersEmulator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES} 
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SpaceInvadersEmulator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

else()
    if(ANDROID)
        add_library(SpaceInvadersEmulator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SpaceInvadersEmulator
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SpaceInvadersEmulator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SpaceInvadersEmulator)
endif()
set_target_properties(SpaceInvadersEmulator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SpaceInvadersEmulator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SpaceInvadersEmulator)
endif()
