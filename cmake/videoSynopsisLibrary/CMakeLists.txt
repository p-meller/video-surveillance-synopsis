cmake_minimum_required(VERSION 3.5)

project(videoSynopsisLibrary VERSION 0.0.1 DESCRIPTION "Video synopsis library")

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS container REQUIRED)
find_package(SQLite3 REQUIRED)


add_library(videoSynopsisLibrary STATIC
        src/Detector/Detector.cpp
        src/Detector/DetectorOutputTypeEnum.h
        src/Detector/DetectorSettings.h
        src/Tracker/KalmanFilter.cpp
        src/Tracker/Track.cpp
        src/Tracker/Tracker.cpp
        src/Database/Database.h
        src/Database/Database.cpp)


IF (WIN32)
    ADD_LIBRARY(ortools STATIC IMPORTED src/Database/Database.cpp)
    SET_TARGET_PROPERTIES(ortools PROPERTIES IMPORTED_LOCATION "E:/or-tools/lib/ortools.lib")
    target_include_directories(videoSynopsisLibrary PRIVATE "E:/or-tools/include")
ENDIF ()

message(DEBUG ${SQLite3_LIBS})

target_link_libraries(videoSynopsisLibrary PRIVATE ${OpenCV_LIBS} ${Boost_LIBS} ortools sqlite3)

set_target_properties(videoSynopsisLibrary PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(videoSynopsisLibrary PROPERTIES INTERFACE_INCLUDE_DIRECTORIES /videoSynopsisLibrary/include/)

target_include_directories(videoSynopsisLibrary PRIVATE src)

