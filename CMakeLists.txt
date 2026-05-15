cmake_minimum_required(VERSION 3.10)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(wxWidgets_ROOT_DIR "C:/wxWidgets")
set(wxWidgets_CONFIGURATION mswu)

find_package(wxWidgets REQUIRED COMPONENTS core base)

include(${wxWidgets_USE_FILE})

include_directories(include ${wxWidgets_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.cpp")

add_executable(DatabaseGUI ${SOURCES})

target_link_libraries(DatabaseGUI ${wxWidgets_LIBRARIES})

add_definitions(-D__WXMSW__ -D__GNUWIN32__ -D_UNICODE)

set(wxWidgets_DLL_DIR "${wxWidgets_ROOT_DIR}/lib/gcc1010_x64_dll")
if(EXISTS ${wxWidgets_DLL_DIR})
    add_custom_command(TARGET DatabaseGUI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${wxWidgets_DLL_DIR}/wxbase32u_gcc1010.dll
        ${wxWidgets_DLL_DIR}/wxmsw32u_core_gcc1010.dll
        ${CMAKE_CURRENT_BINARY_DIR}/)
endif()