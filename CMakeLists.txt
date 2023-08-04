# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (SphereScreenCameraView)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB SRC_FILES
"${PROJECT_SOURCE_DIR}/src/*.h"
"${PROJECT_SOURCE_DIR}/src/*.cpp"
"${PROJECT_SOURCE_DIR}/src/*.c")

#编译可执行文件
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE glm)