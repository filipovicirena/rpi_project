cmake_minimum_required(VERSION 3.5)

project(motion_cam_gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Fix typo in filename: cmaera_stream.h ? camera_stream.h
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    camera_stream.cpp
    camera_stream.h
    motion_detector.cpp
    motion_detector.h
)

# Add executable depending on Qt version
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(motion_cam_gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(motion_cam_gui ${PROJECT_SOURCES})
endif()

# Link required libraries
target_link_libraries(motion_cam_gui
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        ${OpenCV_LIBS}
        pigpio
        pthread
        rt
)

# Target properties
set_target_properties(motion_cam_gui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Install rule
install(TARGETS motion_cam_gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Finalize if Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(motion_cam_gui)
endif()
