cmake_minimum_required(VERSION 3.16)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Поиск SQLite3 (явно указываем цель, если стандартный модуль не находит)
find_package(SQLite3 REQUIRED)
if(NOT TARGET SQLite::SQLite3)
    # Создаём цель для линковки, если она не создалась автоматически
    add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
    set_target_properties(SQLite::SQLite3 PROPERTIES
        IMPORTED_LOCATION "${SQLITE3_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${SQLITE3_INCLUDE_DIR}"
    )
endif()

# Исходные файлы
set(SOURCES
    src/App.cpp
    src/MainFrame.cpp
    src/DatabaseEntity.cpp
    src/DatabaseManager.cpp
    src/Person.cpp
)

# Заголовочные файлы (помогают IDE)
set(HEADERS
    include/App.h
    include/MainFrame.h
    include/DatabaseEntity.h
    include/DatabaseManager.h
    include/Person.h
)

# Создаём исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключаем заголовочные файлы
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Линкуем библиотеки
target_link_libraries(${PROJECT_NAME} 
    ${wxWidgets_LIBRARIES}
    SQLite::SQLite3
)