cmake_minimum_required(VERSION 2.8)

project(hackrf_rx)
SET(MY_TARGET DEMO)
SET(MY_LINK receiver)

#compiler options
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#output_dir
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

MESSAGE("++++++++Start Build++++++++++")

#当前目录的文件
AUX_SOURCE_DIRECTORY(. SRC_LIST)
#添加子目录
ADD_SUBDIRECTORY(src)

#头文件搜索目录
INCLUDE_DIRECTORIES(include,"/usr/local/include/libhackrf")
#链接库搜索路径
LINK_DIRECTORIES(libs)
#设置链接库
SET(EXTRA_LIBS ${EXTRA_LIBS} ${MY_LINK})

#生成可执行文件
ADD_EXECUTABLE(${MY_TARGET} ${SRC_LIST})
#执行文件链接属性
TARGET_LINK_LIBRARIES(${MY_TARGET} ${EXTRA_LIBS},libhackrf.so)

#include_directories("/usr/local/include/libhackrf")



#target_link_libraries(demo libhackrf.so)


