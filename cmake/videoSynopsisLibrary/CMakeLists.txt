cmake_minimum_required(VERSION 3.5)

project(videoSynopsisLibrary VERSION 0.0.1 DESCRIPTION "Video synopsis library")

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS container REQUIRED)

add_library(videoSynopsisLibrary STATIC
        src/Detector/Detector.cpp
        src/Tracker/KalmanFilter.cpp
        src/Tracker/Track.cpp
        src/Tracker/Tracker.cpp
        src/Detector/DetectorSettings.cpp
        src/Detector/DetectorOutputTypeEnum.h)


IF(WIN32)
ADD_LIBRARY(ortools STATIC IMPORTED)
SET_TARGET_PROPERTIES(ortools PROPERTIES IMPORTED_LOCATION "E:/or-tools/lib/ortools.lib")
target_include_directories(videoSynopsisLibrary PRIVATE "E:/or-tools/include")
ENDIF()


target_link_libraries(videoSynopsisLibrary PRIVATE ${OpenCV_LIBS} ${Boost_LIBS} ortools)

set_target_properties(videoSynopsisLibrary PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(videoSynopsisLibrary PROPERTIES INTERFACE_INCLUDE_DIRECTORIES /videoSynopsisLibrary/include/)

target_include_directories(videoSynopsisLibrary PRIVATE src)

