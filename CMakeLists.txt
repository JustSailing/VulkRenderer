cmake_minimum_required(VERSION 3.20)

project(Renderer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Wextra -Wstrict-aliasing -Weffc++")
#set(CMAKE_LIBRARY_PATH ./Vendor/Lib)

include_directories("Vendor/Include")

#include_directories("Vendor/Include/vulkan")
find_library(Vulkan vulkan "Vendor/Lib")
message(${Vulkan})

find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

add_executable(Renderer Src/main.cpp Src/window.cpp Src/shader.cpp Src/device.cpp)
target_link_libraries(Renderer ${Vulkan} X11 pthread dl Xxf86vm Xrandr Xi)

message(${CMAKE_BINARY_DIR})

