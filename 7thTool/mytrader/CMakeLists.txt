# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

project(mytrader)

# 需要指定依赖库目录,多个路径windows使用;分割，linux使用:分割
# -DCMAKE_PREFIX_PATH=D:\share\boost_1_67_0;D:\zqdb\3rd\x64-windows-static;D:\wxWidgets\wxWidgets-3.1.3
MESSAGE(STATUS "CMAKE_PREFIX_PATH: " ${CMAKE_PREFIX_PATH})

# 系统
MESSAGE(STATUS "CMAKE_SYSTEM_NAME:${CMAKE_SYSTEM_NAME} CMAKE_CL_64:${CMAKE_CL_64}")

# 平台
IF(CMAKE_CL_64)
  SET(PLATFORM x64)
ELSE()
  SET(PLATFORM x86)
ENDIF()

MESSAGE(STATUS "PLATFORM: "${PLATFORM})

#一、变量的引用方式是使用“${}"，在IF中，不需要使用这种方式，直接使用变量名即可
#二、自定义变量使用SET(OBJ_NAME xxxx)，使用时${OBJ_NAME}
#三、cmake的常用变量：
#CMAKE_BINARY_DIR,PROJECT_BINARY_DIR,_BINARY_DIR：
#这三个变量内容一致，如果是内部编译，就指的是工程的顶级目录，如果是外部编译，指的就是工程编译发生的目录。
#CMAKE_SOURCE_DIR,PROJECT_SOURCE_DIR,_SOURCE_DIR：
#这三个变量内容一致，都指的是工程的顶级目录。
#CMAKE_CURRENT_BINARY_DIR：外部编译时，指的是target目录，内部编译时，指的是顶级目录
#CMAKE_CURRENT_SOURCE_DIR：CMakeList.txt所在的目录
#CMAKE_CURRENT_LIST_DIR：CMakeList.txt的完整路径
#CMAKE_CURRENT_LIST_LINE：当前所在的行
#CMAKE_MODULE_PATH：如果工程复杂，可能需要编写一些cmake模块，这里通过SET指定这个变量
#LIBRARY_OUTPUT_DIR,BINARY_OUTPUT_DIR：库和可执行的最终存放目录
#PROJECT_NAME：你猜~~
#MESSAGE(STATUS "This is zqdb PROJECT BINARY dir " ${PROJECT_BINARY_DIR})
#MESSAGE(STATUS "This is zqdb CMAKE BINARY dir " ${CMAKE_BINARY_DIR})
#MESSAGE(STATUS "This is zqdb PROJECT SOURCE dir " ${PROJECT_SOURCE_DIR})
#MESSAGE(STATUS "This is zqdb CMAKE SOURCE dir " ${CMAKE_SOURCE_DIR})
#MESSAGE(STATUS "This is zqdb current BINARY dir " ${CMAKE_CURRENT_BINARY_DIR})
#MESSAGE(STATUS "This is zqdb current SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
#MESSAGE(STATUS "This is zqdb current CMAKE SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
#MESSAGE(STATUS "This is zqdb current CMakeList.txt dir " ${CMAKE_CURRENT_LIST_DIR})
#MESSAGE(STATUS "This is zqdb current CMakeList.txt line " ${CMAKE_CURRENT_LIST_LINE})
#四、cmake中调用环境变量
#1.Using $ENV{NAME} : invoke system environment varible.
#We can use "SET(ENV{NAME} value)" as well. note that the "ENV" without "$".

#CMake常用变量
#UNIX 如果为真，表示为 UNIX-like 的系统，包括 AppleOS X 和 CygWin
#WIN32 如果为真，表示为 Windows 系统，包括 CygWin
#APPLE 如果为真，表示为 Apple 系统
#CMAKE_SIZEOF_VOID_P 表示 void* 的大小（例如为 4 或者 8），可以使用其来判断当前构建为 32 位还是 64 位
#CMAKE_CURRENT_LIST_DIR 表示正在处理的CMakeLists.txt 文件的所在的目录的绝对路径（2.8.3 以及以后版本才支持）
#CMAKE_ARCHIVE_OUTPUT_DIRECTORY 用于设置 ARCHIVE 目标的输出路径
#CMAKE_LIBRARY_OUTPUT_DIRECTORY 用于设置 LIBRARY 目标的输出路径
#CMAKE_RUNTIME_OUTPUT_DIRECTORY 用于设置 RUNTIME 目标的输出路径
MESSAGE(STATUS "default output: ${LIBRARY_OUTPUT_DIR} ${BINARY_OUTPUT_DIR} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
MESSAGE(STATUS "default output: ${LIBRARY_OUTPUT_PATH} ${EXECUTABLE_OUTPUT_PATH}")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
MESSAGE(STATUS "${PROJECT_NAME} output: ${LIBRARY_OUTPUT_PATH} ${EXECUTABLE_OUTPUT_PATH}")

# mymodule模块库
add_subdirectory(mymodule)
# myctp模块实现
add_subdirectory(myctp)
# mytrader终端
add_subdirectory(mytrader)
