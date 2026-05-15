cmake_minimum_required(VERSION 3.10)
project(DatabaseGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Executable
add_executable(DatabaseGUI
    src/App.cpp
    src/MainFrame.cpp
    src/DatabaseManager.cpp
    src/Person.cpp
    src/DatabaseEntity.cpp
)

target_include_directories(DatabaseGUI PRIVATE include)
target_link_libraries(DatabaseGUI 
    ${wxWidgets_LIBRARIES}
    SQLite::SQLite3
)