cmake_minimum_required(VERSION 3.10)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(WXWIDGETS_DIR "C:/wxWidgets")
set(WXWIDGETS_LIB_DIR "${WXWIDGETS_DIR}/lib/gcc810_x64_dll")

set(SQLITE3_DIR "C:/SQLite")

include_directories(
    include
    ${WXWIDGETS_DIR}/include
    ${SQLITE3_DIR}
)

file(GLOB SOURCES "src/*.cpp")

add_executable(DatabaseGUI ${SOURCES})

target_link_libraries(DatabaseGUI
    ${WXWIDGETS_LIB_DIR}/libwxbase32u.a
    ${WXWIDGETS_LIB_DIR}/libwxmsw32u_core.a
    ${SQLITE3_DIR}/sqlite3.dll
)

add_definitions(-D__WXMSW__ -D__GNUWIN32__ -D_UNICODE)

add_custom_command(TARGET DatabaseGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SQLITE3_DIR}/sqlite3.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
)