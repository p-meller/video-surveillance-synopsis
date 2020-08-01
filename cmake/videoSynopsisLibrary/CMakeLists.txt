cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)

project(videoSynopsisLibrary VERSION 0.0.1 DESCRIPTION "Video synopsis library")

#SET("OpenCV_DIR" "/usr/share/OpenCV")
find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS container REQUIRED)
#find_package( Op REQUIRED )

add_library(videoSynopsisLibrary SHARED
        src/Detector.cpp
        src/KalmanFilter.cpp
        src/Track.cpp
        src/Tracker.cpp
        )

message(STATUS ${OpenCV_LIBS})

target_link_libraries(videoSynopsisLibrary PRIVATE ${OpenCV_LIBS} ${Boost_LIBS} ortools)


set_target_properties(videoSynopsisLibrary PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(videoSynopsisLibrary PROPERTIES INTERFACE_INCLUDE_DIRECTORIES /videoSynopsisLibrary/include/videoSynopsisLib.h)

target_include_directories(videoSynopsisLibrary PRIVATE src)