cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(8583MSG_DEC C CXX)
if(WIN32)
	set(PRODIR "E:\\DevProjects\\8583MSG_DEC")
	set(QTDIR "E:\\Qt\\Qt5.12.2")
	set(CYGWINDIR "D:\\cygwin64")
endif()
set(CMAKE_SOURCE_DIR  "src")
set(CMAKE_BINARY_DIR  "output")

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
# place binaries and libraries according to GNU standards
# include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PRODIR}/Lib/x64)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PRODIR}/Lib/x64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY .)

if(WIN32)
	set(CMAKE_C_COMPILER ${QTDIR}/Tools/mingw730_64/bin/gcc.exe)
	set(CMAKE_CXX_COMPILER ${QTDIR}/Tools/mingw730_64/bin/g++.exe)
	set(CMAKE_MAKE_PROGRAM ${QTDIR}/Tools/mingw730_64/bin/mingw32-make.exe)
	set(CMAKE_RC_COMPILER ${QTDIR}/Tools/mingw730_64/bin/windres.exe)
	set(CMAKE_SH ${CYGWINDIR}/sh.exe)
	set(CMAKE_PREFIX_PATH ${QTDIR}/mingw73_64/lib/cmake)
endif()
# example how to set c/c++ compiler flags for GNU
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
#	set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Wwrite-strings -Wno-unused")
	set(CMAKE_C_FLAGS         "${CMAKE_C_FLAGS} -Wall")
    set(CMAKE_C_FLAGS_DEBUG   "-O0 -g")
    set(CMAKE_C_FLAGS_RELEASE "-O3")
#	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
	set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wno-sign-compare -Woverloaded-virtual -Wwrite-strings -Wno-unused")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Test REQUIRED)

# Populate a CMake variable with the sources
include_directories(${PRODIR}/Include ${PRODIR}/Include/adamhh)
set(decRES    
	${CMAKE_SOURCE_DIR}/dec/test.c
	${CMAKE_SOURCE_DIR}/dec/tools.c
	${CMAKE_SOURCE_DIR}/dec/decoder.c
)
set(dumpRES
	${CMAKE_SOURCE_DIR}/dump/test.c
	${CMAKE_SOURCE_DIR}/dump/loadDll.c
	${CMAKE_SOURCE_DIR}/dump/list.c
	${CMAKE_SOURCE_DIR}/dump/print.c
	${CMAKE_SOURCE_DIR}/dump/callback.c
	${CMAKE_SOURCE_DIR}/dump/userbuff.c
)
set(GUIRES
	${CMAKE_SOURCE_DIR}/GUI/main.cpp
	${CMAKE_SOURCE_DIR}/GUI/mainWindow.cpp	
	${CMAKE_SOURCE_DIR}/GUI/interfaceDialog.cpp
	${CMAKE_SOURCE_DIR}/GUI/icons.cpp
	${CMAKE_SOURCE_DIR}/GUI/dataItem.cpp
	${CMAKE_SOURCE_DIR}/GUI/dataItemModel.cpp
	${CMAKE_SOURCE_DIR}/GUI/ThreadTreeView.h
)
# Tell CMake to create the helloworld executable
add_library(decl STATIC ${decRES})
add_library(dumpl STATIC ${dumpRES})
add_executable(dec ${decRES})
add_executable(dump ${dumpRES})
add_executable(gui WIN32 ${GUIRES} ${CMAKE_SOURCE_DIR}/project.rc)

target_link_libraries(dump ${PRODIR}/Lib/x64/wpcap.lib)
target_link_libraries(gui ${PRODIR}/Lib/x64/libdumpl.a ${PRODIR}/Lib/x64/libdecl.a ${PRODIR}/Lib/x64/wpcap.lib Qt5::Core Qt5::Widgets Qt5::Test)
