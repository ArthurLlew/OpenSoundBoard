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
# Try compilers at compiling library rather executable (better for cross compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
# The project name (also the name of resulting .exe file)
project(SoundBoard)

# Defines list of source files
set(SOURCES main.cpp MainWindow.cpp DeviceTab.cpp AudioTrack.cpp AudioPlayerManagers.cpp AudioPlayers.cpp AudioTrackContext.cpp
    WidgetMessages.cpp)
# Defines names of static libraries
set(LIBS Qt5Core Qt5Widgets Qt5Gui portaudio avutil avcodec avformat swresample)

# Adds static libraries
foreach(LIB IN LISTS ${LIBS})
    add_library(LIB STATIC IMPORTED)
    string(CONCAT LIB_LOCATION "-l" LIB)
    set_target_properties(LIB PROPERTIES
        # This string will be placed in compiler options (can either be a full path to library or jsut a -l flag with library name)
        IMPORTED_LOCATION LIB_LOCATION
        INTERFACE_INCLUDE_DIRECTORIES "C:/Apps/msys64/ucrt64/include")
endforeach()

# Is used by AUTOMOC
find_package(Qt5 REQUIRED QUIET COMPONENTS Core Widgets Gui)
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt5Core_VERSION_MAJOR "${Qt5Core_VERSION_MAJOR}")
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY Qt5Core_VERSION_MINOR "${Qt5Core_VERSION_MAJOR}")

# Executable name and sources (WIN32 disables console window)
add_executable(SoundBoard WIN32 ${SOURCES})
add_executable(SoundBoard_debug ${SOURCES})
# Is used to process all QT macros with MOC to generate additional code
set_property(TARGET SoundBoard PROPERTY AUTOMOC ON)
set_property(TARGET SoundBoard_debug PROPERTY AUTOMOC ON)
# Include path for .hpp files
target_include_directories(SoundBoard PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(SoundBoard_debug PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Libraries linking
target_link_libraries(SoundBoard PUBLIC ${LIBS})
target_link_libraries(SoundBoard_debug PUBLIC ${LIBS})