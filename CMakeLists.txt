cmake_minimum_required(VERSION 3.1.0)

project(8583MSG_DEC)
set(PRODIR "F:\\DevProjects\\8583MSG_DEC")

set(CMAKE_SOURCE_DIR  "src")
set(CMAKE_BINARY_DIR  "output")
set(CMAKE_MAKE_PROGRAM "C:\\Qt\\Qt5.12.2\\Tools\\mingw730_64\\bin\\mingw32-make.exe")
set(CMAKE_RC_COMPILER "C:\\Qt\\Qt5.12.2\\Tools\\mingw730_64\\bin\\windres.exe")
set(CMAKE_SH "D:\\cygwin64\\bin\\sh.exe")
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.12.2\\5.12.2\\mingw73_64\\lib\\cmake")

find_package(Qt5Widgets CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(decRES
    ${CMAKE_SOURCE_DIR}/dec/8583.h
	${CMAKE_SOURCE_DIR}/dec/8583_main.c
	${CMAKE_SOURCE_DIR}/dec/tools.c
	${CMAKE_SOURCE_DIR}/dec/decoder.c
)
set(dumpRES
	${CMAKE_SOURCE_DIR}/dump/test.c
	${CMAKE_SOURCE_DIR}/dump/loadNpcapDlls.c
	${CMAKE_SOURCE_DIR}/dump/list.c
	${CMAKE_SOURCE_DIR}/dump/print.c
	${CMAKE_SOURCE_DIR}/dump/callback.c
	${CMAKE_SOURCE_DIR}/dump/userbuff.c
)
# Tell CMake to create the helloworld executable
add_executable(dec ${decRES})
add_executable(dump ${dumpRES})

include_directories(${PRODIR}/Include ${PRODIR}/Include/adamhh)
#link_directories("F:\\DevProjects\\8583MSG_DEC\\Lib\\x64")
target_link_libraries(dump "F:\\DevProjects\\8583MSG_DEC\\Lib\\x64\\wpcap.lib")