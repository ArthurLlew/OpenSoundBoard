# Min CMake version
cmake_minimum_required(VERSION 4.0)

# Sets C/C++ compilers
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
# C/C++ language levels
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Try compilers for compiling library rather than executable (better for cross compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
# Project name
project(OpenSoundBoard)

# Application icon resources target
set(APP_ICON_ICO ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.ico)     # resources
set(APP_ICON_RC ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc)       # resources
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.o)           # output
add_custom_command(
    OUTPUT ${APP_ICON}                                              # ouput file
    DEPENDS ${APP_ICON_RC} ${APP_ICON_ICO}                          # dependencies
    COMMAND windres ${APP_ICON_RC} -o ${APP_ICON}                   # command
    COMMENT "Creating app icon")                                    # command line message
add_custom_target(app_icon ALL DEPENDS ${APP_ICON})

# Create resources target
add_executable(embedder embedder.cpp)                               # executable to be compiled here and then run in command ahead
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/style.qss)      # resources
set(RESOURCES_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/src/embedded.h)    # output
add_custom_command(
    OUTPUT ${RESOURCES_HEADER}                                      # ouput file
    DEPENDS embedder ${RESOURCES}                                   # dependencies
    COMMAND embedder ${RESOURCES} ${RESOURCES_HEADER}               # command
    COMMENT "Embedding resources")                                  # command line message
add_custom_target(resources ALL DEPENDS ${RESOURCES_HEADER})

# List of source files (.c/.cpp)
set(SOURCES src/main.cpp src/MainWindow.cpp src/WidgetMessageBoxing/WidgetWarning.cpp src/DeviceTab.cpp src/AudioTrack.cpp
            src/AudioPlayerWidgets/AudioPlayerWidget.cpp src/AudioPlayerWidgets/MicrophonePlayerWidget.cpp
            src/AudioPlayerWidgets/MediaFilesPlayerWidget.cpp
            src/AudioPlayers/AudioPlayer.cpp src/AudioPlayers/MicrophonePlayer.cpp src/AudioPlayers/MediaFilesPlayer.cpp
            src/AudioPlayers/AudioTrackReader.cpp)

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

# Define and find Qt packages
set(LIBS_QT6 Core Widgets Gui Multimedia)
find_package(Qt6 REQUIRED COMPONENTS ${LIBS_QT6})

# AUTOMOC properties
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt6Core_VERSION_MAJOR "${Qt6Core_VERSION_MAJOR}")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt6Core_VERSION_MINOR "${Qt6Core_VERSION_MAJOR}")

# Executables (executable name, sources needed for its compilation and application icon)
add_executable(OpenSoundBoard WIN32 ${SOURCES} ${APP_ICON}) # (WIN32 disables console window)
add_executable(OpenSoundBoard_debug ${SOURCES} ${APP_ICON})

# Embeded resources using Qt
find_package(Qt6 REQUIRED COMPONENTS Core)
set(EMBEDED_RESOURCES resources/arrow_up.png resources/arrow_down.png resources/dragged_track.png)
qt_add_resources(OpenSoundBoard "resources"
    PREFIX "/"
    FILES ${EMBEDED_RESOURCES})
qt_add_resources(OpenSoundBoard_debug "resources"
    PREFIX "/"
    FILES ${EMBEDED_RESOURCES})

# Is used to process all Qt macros with MOC (generate additional Qt code)
set_property(TARGET OpenSoundBoard PROPERTY AUTOMOC ON)
set_property(TARGET OpenSoundBoard_debug PROPERTY AUTOMOC ON)
# Include path
target_include_directories(OpenSoundBoard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(OpenSoundBoard_debug PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
# Libraries linking
target_link_libraries(OpenSoundBoard PRIVATE ${LIBS} $<LIST:TRANSFORM,${LIBS_QT6},PREPEND,Qt6::>)
target_link_libraries(OpenSoundBoard_debug PRIVATE ${LIBS} $<LIST:TRANSFORM,${LIBS_QT6},PREPEND,Qt6::>)
# Resulting executable should depend on header with resources (so header file is generated before them)
add_dependencies(OpenSoundBoard resources app_icon)
add_dependencies(OpenSoundBoard_debug resources app_icon)