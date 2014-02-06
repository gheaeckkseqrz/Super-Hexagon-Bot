ADD_DEFINITIONS(
    -std=c++11 # Or -std=c++0x
    # -W -Wall -Werror
    # Other flags
)

cmake_minimum_required(VERSION 2.8)
project(DisplayImage)
find_package( OpenCV REQUIRED )
add_executable( DisplayImage src/main.cpp src/Edge.cpp src/SHBot.cpp src/Option.cpp)
target_link_libraries( DisplayImage ${OpenCV_LIBS} -lXtst -lX11)
