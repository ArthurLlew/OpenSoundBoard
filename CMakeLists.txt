# Min CMake version
cmake_minimum_required(VERSION 3.3)

# Sets C/C++ compilers
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
# C/C++ language level
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Try compilers for compiling library rather than executable (better for cross compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
# Project name
project(OpenSoundBoard)

# Create application icon target
set(APP_ICON_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.ico)
set(APP_ICON_RC ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc)
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.o)
add_custom_command(
    OUTPUT ${APP_ICON}
    DEPENDS ${APP_ICON_ICO} ${APP_ICON_RC}
    COMMAND windres ${APP_ICON_RC} -o ${APP_ICON}
    COMMENT "Creating app icon")
add_custom_target(app_icon ALL DEPENDS ${APP_ICON})

# Create resources target
add_executable(embedder embedder.cpp)
set(RESOURCES_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/embedded.h)
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/style.qss)
add_custom_command(
    OUTPUT ${RESOURCES_HEADER}
    DEPENDS embedder ${RESOURCES}
    COMMAND embedder ${RESOURCES} ${RESOURCES_HEADER}
    COMMENT "Embedding resources")
add_custom_target(resources ALL DEPENDS ${RESOURCES_HEADER})

# Define list of source files
set(SOURCES src/main.cpp src/MainWindow.cpp src/WidgetMessages.cpp src/DeviceTab.cpp src/AudioTrack.cpp
            src/AudioPlayerManagers/AudioPlayerManager.cpp src/AudioPlayerManagers/MicrophonePlayerManager.cpp
            src/AudioPlayerManagers/MediaFilesPlayerManager.cpp
            src/AudioPlayers/AudioPlayer.cpp src/AudioPlayers/MicrophonePlayer.cpp src/AudioPlayers/MediaFilesPlayer.cpp
            src/AudioPlayers/AudioTrackContext.cpp)
# Define names of static libraries
set(LIBS dwmapi avutil avcodec avformat swresample)
# Add static libraries
foreach(LIB IN LISTS ${LIBS})
    add_library(LIB STATIC IMPORTED)
    # This string will be placed in compiler options (can either be a full path to library or just a -l flag with library name)
    string(CONCAT LIB_LOCATION "-l" LIB)
    # Set properties
    set_target_properties(LIB PROPERTIES
        IMPORTED_LOCATION LIB_LOCATION
        INTERFACE_INCLUDE_DIRECTORIES "C:/Apps/msys64/ucrt64/include")
endforeach()
# Find Qt6
set(LIBS_QT6 Core Widgets Gui Multimedia)
find_package(Qt6 REQUIRED COMPONENTS ${LIBS_QT6})
# AUTOMOC
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt6Core_VERSION_MAJOR "${Qt6Core_VERSION_MAJOR}")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt6Core_VERSION_MINOR "${Qt6Core_VERSION_MAJOR}")

# Executable name and sources (WIN32 disables console window)
add_executable(OpenSoundBoard WIN32 ${SOURCES} ${APP_ICON})
add_executable(OpenSoundBoard_debug ${SOURCES} ${APP_ICON})

# Is used to process all QT macros with MOC to generate additional code
set_property(TARGET OpenSoundBoard PROPERTY AUTOMOC ON)
set_property(TARGET OpenSoundBoard_debug PROPERTY AUTOMOC ON)
# Include path for .hpp files
target_include_directories(OpenSoundBoard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(OpenSoundBoard_debug PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Libraries linking
target_link_libraries(OpenSoundBoard PRIVATE ${LIBS} $<LIST:TRANSFORM,${LIBS_QT6},PREPEND,Qt6::>)
target_link_libraries(OpenSoundBoard_debug PRIVATE ${LIBS} $<LIST:TRANSFORM,${LIBS_QT6},PREPEND,Qt6::>)
# Resulting executable should depend on header with resources (so header file is generated before them)
add_dependencies(OpenSoundBoard resources app_icon)
add_dependencies(OpenSoundBoard_debug resources app_icon)