cmake_minimum_required(VERSION 3.17)
project(videoSynopsisConsole)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )

add_executable(videoSynopsisConsole main.cpp)

target_link_libraries(videoSynopsisConsole PRIVATE ${OpenCV_LIBS} videoSynopsisLibrary)
target_include_directories(videoSynopsisConsole PRIVATE ../videoSynopsisLibrary/include)
