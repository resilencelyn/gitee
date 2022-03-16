cmake_minimum_required(VERSION 3.4.1)

project(mytrader)

MESSAGE(STATUS "This is ${PROJECT_NAME} PROJECT BINARY dir " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} CMAKE BINARY dir " ${CMAKE_BINARY_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} PROJECT SOURCE dir " ${PROJECT_SOURCE_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} CMAKE SOURCE dir " ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "This is ${PROJECT_NAME} current BINARY dir " ${CMAKE_CURRENT_BINARY_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} current SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} current CMAKE SOURCE dir " ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} current CMakeList.txt dir " ${CMAKE_CURRENT_LIST_DIR})
MESSAGE(STATUS "This is ${PROJECT_NAME} current CMakeList.txt line " ${CMAKE_CURRENT_LIST_LINE})

SET(PROJECT_VER_MAJOR 2)
SET(PROJECT_VER_MINOR 3)
SET(PROJECT_VER_RELEASE 2)
SET(PROJECT_VER_BUILD 2)
include(${CMAKE_SOURCE_DIR}/../zqdb/cmake/version.cmake)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)   #导出clangd需要的文件，用于智能提示和基于语议的补全

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
MESSAGE(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
MESSAGE(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
add_definitions(-D_DEBUG)
ENDIF()

if (MSVC)
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif(MSVC)

# add location of platform.hpp for Windows builds
if(WIN32)
  add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_definitions(-DWIN32 -D_WINDOWS)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1)
  add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS=1)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions(-D__WXMSW__)
  add_definitions(-DNOPCH)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  # Same name on 64bit systems
  link_libraries(ws2_32.lib)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows") 
endif()

IF(WIN32)
	#需要兼容XP时,定义_WIN32_WINNT 0x0501
	ADD_DEFINITIONS(-D_WIN32_WINNT=0x0602)
	ADD_DEFINITIONS(-DLIB_ZQDB_API)
	ADD_DEFINITIONS(-DLIB_CALC_API)
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	#SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
	#SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
ELSE()
	ADD_DEFINITIONS(-fpermissive)
	ADD_DEFINITIONS(-fPIC)
	#-w的意思是关闭编译时的警告，也就是编译后不显示任何warning
	#-Wall选项意思是编译后显示所有警告
	#-W选项类似-Wall，会显示警告，但是只显示编译器认为会出现错误的警告
	#调试信息格式主要有下面几种：stabs，COFF，PE-COFF，OMF，IEEE-695和DWARF
	#其中DWARF在Linux中被普遍使用，dwarf2对dwarf1的改变很大，dwarf3大多是对dwarf2的扩充，可以支持C、C++、JAVA、Fortran等语言
	#使用readelf –w* transfer命令，*是调试节名的第一个字母，如-wi就是查看.debug_info节的内容，-wl就是查看.debug_line节的内容
	#-g、-ggdb、-g3和-ggdb3，-g产生OS native format的debug信息，GDB可以使用之。而-ggdb产生的debug信息更倾向于给GDB使用的
	#如果你用的GDB调试器，那么使用-ggdb选项。如果是其他调试器，则使用-g。3只是debug信息级别，3这个级别可以调试宏。
	#SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -w -gdwarf-2 -ggdb3")
	#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall -DNODEBUG -gdwarf-2 -ggdb")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -W -g3")
	SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -W")
	SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -W -gdwarf-3 -g")
	SET(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -W")
  	INCLUDE_DIRECTORIES(/usr/local/include)
	LINK_DIRECTORIES(/usr/local/lib)
ENDIF()
CMAKE_POLICY(SET CMP0015 NEW)

#设置需要的Boost组件
#SET(BOOST_COMPONENTS log_setup log regex date_time chrono system filesystem thread)
#使用cmake find_package 查找boost库位置
#FIND_PACKAGE(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
FIND_PACKAGE(Boost REQUIRED)
IF(Boost_FOUND)
	MESSAGE(STATUS "Boost library status:")
	MESSAGE(STATUS "     version: ${Boost_VERSION}")
ELSE()
	#MESSAGE(FATAL_ERROR "BOOST library not found")
	if(BOOST_ROOT)
		set(Boost_INCLUDE_DIRS ${BOOST_ROOT})
	else()
		set(Boost_INCLUDE_DIRS ../../boost_1_66_0)
	endif()
ENDIF()
IF(WIN32)
  	SET(Boost_LIBRARY_DIRS "$ENV{BOOST_ROOT}/lib64")
ELSE()
	set(Boost_LIBRARY_DIRS $ENV{BOOST_ROOT}/output/stage/linux/lib)
	#使用log动态库需要定义BOOST_LOG_DYN_LINK
  	#ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
	FIND_LIBRARY(BOOST_LOG_SETUP boost_log_setup ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_LOG boost_log ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_REGEX boost_regex ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_DATE_TIME boost_date_time ${Boost_LIBRARY_DIRS})
	FIND_LIBRARY(BOOST_CHRONO boost_chrono ${Boost_LIBRARY_DIRS})
	SET (EXTRA_LIBS ${EXTRA_LIBS} ${BOOST_LOG_SETUP} ${BOOST_LOG} ${BOOST_REGEX} ${BOOST_DATE_TIME} ${BOOST_CHRONO}
		boost_locale boost_system boost_filesystem boost_serialization boost_thread -pthread dl)
ENDIF()
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
MESSAGE(STATUS "boost include path: ${Boost_INCLUDE_DIRS}")
MESSAGE(STATUS "boost library path: ${Boost_LIBRARY_DIRS}")

find_package(Protobuf REQUIRED)
IF(PROTOBUF_FOUND)
	MESSAGE(STATUS "protobuf library status:")
	MESSAGE(STATUS "     version: ${PROTOBUF_VERSION}")
	MESSAGE(STATUS "     include path: ${PROTOBUF_INCLUDE_DIR}")
	MESSAGE(STATUS "     library path: ${PROTOBUF_LIBRARIES}")
	INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})
  LINK_DIRECTORIES(${PROTOBUF_INCLUDE_DIR}/../${CMAKE_BUILD_TYPE}/lib)
	SET(EXTRA_LIBS ${EXTRA_LIBS} ${PROTOBUF_LIBRARIES})
ELSE()
	MESSAGE(FATAL_ERROR "protobuf library not found")
ENDIF()
#target_link_libraries(mdb PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

find_package(wxWidgets COMPONENTS qa core base aui ribbon xml html propgrid richtext stc scintilla)
if(wxWidgets_FOUND)
	MESSAGE(STATUS "     include path: ${wxWidgets_USE_FILE}")
	MESSAGE(STATUS "     library path: ${wxWidgets_LIBRARIES}")
  include(${wxWidgets_USE_FILE})
  SET(EXTRA_LIBS ${EXTRA_LIBS} ${wxWidgets_LIBRARIES})
  #wxFreeChart
  INCLUDE_DIRECTORIES(../../zqdb/3rd/wxFreeChart/include)
	IF(WIN32)
		IF(CMAKE_CL_64)
			LINK_DIRECTORIES(../../zqdb/3rd/wxFreeChart/lib/vc_x64_lib)
		ELSE()
			LINK_DIRECTORIES(../../zqdb/3rd/wxFreeChart/lib/vc_lib)
		ENDIF()
		IF(CMAKE_BUILD_TYPE STREQUAL Debug)
			SET (EXTRA_LIBS ${EXTRA_LIBS} wxFreeChart17ud)
		ELSE()
			SET (EXTRA_LIBS ${EXTRA_LIBS} wxFreeChart17u)
		ENDIF()
	ELSE()
		MESSAGE(FATAL_ERROR "wxFreeChart library not found")
	ENDIF()
ELSE()
	MESSAGE(FATAL_ERROR "wxWidgets library not found")
ENDIF()

IF(WIN32)
	INCLUDE_DIRECTORIES(../../zqdb/3rd ../../zqdb/include ../include ../src)
	LINK_DIRECTORIES(../../zqdb/bin/${CMAKE_SYSTEM_NAME}/${PLATFORM})
	
	SET (EXTRA_LIBS ${EXTRA_LIBS} bz mdbase net zqdb calc view techview mymodule myctp
	#mysqlcppconn 
	#jwtd libeay32 ssleay32
	)
ELSE()
	INCLUDE_DIRECTORIES(../../zqdb/3rd ../../zqdb/include ../include ../src)
	LINK_DIRECTORIES(../../zqdb/bin/${CMAKE_SYSTEM_NAME}/${PLATFORM})
	SET (EXTRA_LIBS ${EXTRA_LIBS} bz mdbase net zqdb calc view techview myctp
	#mysqlcppconn jwt 
	#crypto ssl 
	pthread dl
	)
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME}
../../zqdb/src/wxutil.cpp
base.cpp
utility.cpp
settings.cpp
object.cpp
smartkb.cpp
myapp.cpp
mylog.cpp
mylogdlg.cpp
mytaskbar.cpp
mydatamodel.cpp
mydataview.cpp
myprops.cpp
mycalcdlg.cpp
mysettingsdlg.cpp
mylogindlg.cpp
mysmartkbdlg.cpp
mybaseframe.cpp
mycalcframe.cpp
mytechframe.cpp
mystrategyframe.cpp
myframe.cpp
mytrader.rc
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE ${EXTRA_LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../zqdb/bin/${CMAKE_SYSTEM_NAME}/${PLATFORM})
