cmake_minimum_required(VERSION 3.5)

project(videoSynopsisGui LANGUAGES CXX)

find_package(OpenCV REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_QMLTYPES)
set(QML_IMPORT_NAME = com.videoSynopsisGui.classes)
set(QML_IMPORT_MAJOR_VERSION = 1)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Multimedia Quick REQUIRED)

if (ANDROID)
    add_library(videoSynopsisGui SHARED
            src/main.cpp
            src/qml.qrc
            src/PreviewFilter.cpp
            src/PreviewImageProvider.cpp
            )
else ()
    add_executable(videoSynopsisGui
            src/main.cpp
            src/qml.qrc
            src/PreviewFilter.cpp
            src/PreviewImageProvider.cpp)
endif ()

target_include_directories(videoSynopsisGui PRIVATE src ../videoSynopsisLibrary/include)

target_compile_definitions(videoSynopsisGui
        PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(videoSynopsisGui
        PRIVATE Qt5::Core Qt5::Multimedia Qt5::Quick ${OpenCV_LIBS} videoSynopsisLibrary)

