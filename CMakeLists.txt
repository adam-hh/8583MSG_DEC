cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(8583MSG_DEC C CXX)

if(WIN32)
	set(PRODIR "F:\\DevProjects\\8583MSG_DEC")
	set(QTDIR "C:\\Qt\\Qt5.12.2")
	set(CYGWINDIR "D:\\cygwin64")
endif()

# detect operating system and host processor
message(STATUS "We are on a ${CMAKE_SYSTEM_NAME} system")
message(STATUS "The host processor is ${CMAKE_HOST_SYSTEM_PROCESSOR}")
# place binaries and libraries according to GNU standards
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib/x64)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Lib/x64)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
	set(CMAKE_C_COMPILER ${QTDIR}/Tools/mingw730_64/bin/gcc.exe)
	set(CMAKE_CXX_COMPILER ${QTDIR}/Tools/mingw730_64/bin/g++.exe)
	set(CMAKE_MAKE_PROGRAM ${QTDIR}/Tools/mingw730_64/bin/mingw32-make.exe)
	set(CMAKE_RC_COMPILER ${QTDIR}/Tools/mingw730_64/bin/windres.exe)
	set(CMAKE_SH ${CYGWINDIR}/sh.exe)	
endif()
# set c/c++ compiler flags for GNU
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
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
set(CMAKE_PREFIX_PATH ${QTDIR}/mingw73_64/lib/cmake)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Test CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/Include ${CMAKE_SOURCE_DIR}/Include/adamhh)
link_directories(${CMAKE_SOURCE_DIR}/Lib/x64)
set(decSRC    
	${CMAKE_SOURCE_DIR}/src/dec/test.c
	${CMAKE_SOURCE_DIR}/src/dec/tools.c
	${CMAKE_SOURCE_DIR}/src/dec/decoderJL.c
	${CMAKE_SOURCE_DIR}/src/dec/decoderYS.c
	${CMAKE_SOURCE_DIR}/src/dec/decoderYL.c
)
set(dumpSRC
	${CMAKE_SOURCE_DIR}/src/dump/test.c
	${CMAKE_SOURCE_DIR}/src/dump/loadDll.c
	${CMAKE_SOURCE_DIR}/src/dump/list.c
	${CMAKE_SOURCE_DIR}/src/dump/print.c
	${CMAKE_SOURCE_DIR}/src/dump/callback.c
	${CMAKE_SOURCE_DIR}/src/dump/userbuff.c
)
set(GUISRC
	${CMAKE_SOURCE_DIR}/src/GUI/main.cpp
	${CMAKE_SOURCE_DIR}/src/GUI/mainWindow.cpp	
	${CMAKE_SOURCE_DIR}/src/GUI/interfaceDialog.cpp
	${CMAKE_SOURCE_DIR}/src/GUI/icons.cpp
	${CMAKE_SOURCE_DIR}/src/GUI/dataItem.cpp
	${CMAKE_SOURCE_DIR}/src/GUI/dataItemModel.cpp
	${CMAKE_SOURCE_DIR}/src/GUI/ThreadTreeView.h
)
# Tell CMake to create the helloworld executable
add_library(decl SHARED ${decSRC})
add_library(dumpl SHARED ${dumpSRC})
add_executable(dec ${decSRC})
add_executable(dump ${dumpSRC})
add_executable(gui WIN32 ${GUISRC} ${CMAKE_SOURCE_DIR}/src/project.rc)

target_link_libraries(dump wpcap.lib)
target_link_libraries(dumpl wpcap.lib)
target_link_libraries(gui Qt5::Core Qt5::Widgets Qt5::Test -ldumpl -ldecl wpcap.lib)
