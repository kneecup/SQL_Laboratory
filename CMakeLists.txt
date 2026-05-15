cmake_minimum_required(VERSION 3.10)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути
set(WXWIDGETS_DIR "C:/wxWidgets")
set(WXWIDGETS_LIB_DIR "${WXWIDGETS_DIR}/lib/gcc810_x64_dll")
set(WXWIDGETS_SETUP_DIR "${WXWIDGETS_LIB_DIR}/mswu")
set(SQLITE3_DIR "C:/SQLite")

# Заголовочные файлы
include_directories(
    include
    ${WXWIDGETS_SETUP_DIR}
    ${WXWIDGETS_DIR}/include
    ${SQLITE3_DIR}
)

# Исходные файлы
file(GLOB SOURCES "src/*.cpp")
list(APPEND SOURCES "${SQLITE3_DIR}/sqlite3.c")

# Создание EXE
add_executable(DatabaseGUI ${SOURCES})

# Линковка библиотек
target_link_libraries(DatabaseGUI
    ${WXWIDGETS_LIB_DIR}/libwxbase32u.a
    ${WXWIDGETS_LIB_DIR}/libwxmsw32u_core.a
)

# Определения
add_definitions(
    -D__WXMSW__
    -D__GNUWIN32__
    -D_UNICODE
    -DWXUSINGDLL
)

# Копирование DLL
add_custom_command(TARGET DatabaseGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WXWIDGETS_LIB_DIR}/wxbase32u_gcc810.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WXWIDGETS_LIB_DIR}/wxmsw32u_core_gcc810.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
)