cmake_minimum_required(VERSION 3.5)
project(videoSynopsisConsole)

find_package(OpenCV REQUIRED)

add_executable(videoSynopsisConsole main.cpp)

target_link_libraries(videoSynopsisConsole PRIVATE ${OpenCV_LIBS} videoSynopsisLibrary)
target_include_directories(videoSynopsisConsole PRIVATE ../videoSynopsisLibrary/include)
