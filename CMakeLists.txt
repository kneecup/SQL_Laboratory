cmake_minimum_required(VERSION 3.10)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# УКАЖИТЕ ВАШИ ПУТИ (ПРОВЕРЬТЕ НАЗВАНИЯ ПАПОК!)
# ============================================
set(WXWIDGETS_DIR "C:/wxWidgets")
set(WXWIDGETS_VERSION "32u")  # Unicode версия
set(WXWIDGETS_CONFIG "mswu")   # Windows + Unicode

# Пытаемся определить правильную папку lib
if(EXISTS "${WXWIDGETS_DIR}/lib/gcc810_x64_dll")
    set(WXWIDGETS_LIB_DIR "${WXWIDGETS_DIR}/lib/gcc810_x64_dll")
elseif(EXISTS "${WXWIDGETS_DIR}/lib/gcc730_x64_dll")
    set(WXWIDGETS_LIB_DIR "${WXWIDGETS_DIR}/lib/gcc730_x64_dll")
else()
    message(FATAL_ERROR "Не найдена папка lib в ${WXWIDGETS_DIR}")
endif()

set(WXWIDGETS_INCLUDE_DIR "${WXWIDGETS_DIR}/include")
set(WXWIDGETS_SETUP_DIR "${WXWIDGETS_LIB_DIR}/${WXWIDGETS_CONFIG}")

# SQLite
set(SQLITE3_DIR "C:/SQLite")

# ============================================
# ЗАГОЛОВОЧНЫЕ ФАЙЛЫ (ВАЖНО: порядок имеет значение!)
# ============================================
include_directories(
    include
    ${WXWIDGETS_SETUP_DIR}      # Сначала папка с setup.h
    ${WXWIDGETS_INCLUDE_DIR}    # Потом основная папка include
    ${SQLITE3_DIR}
)

# ============================================
# ИСХОДНЫЕ ФАЙЛЫ
# ============================================
file(GLOB SOURCES "src/*.cpp")

# ============================================
# СОЗДАНИЕ EXE
# ============================================
add_executable(DatabaseGUI ${SOURCES})

# ============================================
# БИБЛИОТЕКИ ДЛЯ ЛИНКОВКИ
# ============================================
target_link_libraries(DatabaseGUI
    ${WXWIDGETS_LIB_DIR}/libwxbase${WXWIDGETS_VERSION}.a
    ${WXWIDGETS_LIB_DIR}/libwxmsw${WXWIDGETS_VERSION}_core.a
    ${SQLITE3_DIR}/sqlite3.dll
)

# ============================================
# ОПРЕДЕЛЕНИЯ ДЛЯ WXWIDGETS
# ============================================
add_definitions(
    -D__WXMSW__
    -D__GNUWIN32__
    -D_UNICODE
    -DWXUSINGDLL
)

# ============================================
# КОПИРУЕМ DLL
# ============================================
add_custom_command(TARGET DatabaseGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WXWIDGETS_LIB_DIR}/wxbase${WXWIDGETS_VERSION}_gcc*.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WXWIDGETS_LIB_DIR}/wxmsw${WXWIDGETS_VERSION}_core_gcc*.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SQLITE3_DIR}/sqlite3.dll
        ${CMAKE_CURRENT_BINARY_DIR}/
)