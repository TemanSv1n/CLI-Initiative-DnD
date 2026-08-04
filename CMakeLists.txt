cmake_minimum_required(VERSION 3.16)

project(
    cliniti
    VERSION 1.0.0
    DESCRIPTION "Simple CLI initiative tracker for DnD"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Curses REQUIRED)

add_executable(cliniti
    src/main.cpp
)

target_include_directories(cliniti
    PRIVATE
        ${CURSES_INCLUDE_DIRS}
)

target_link_libraries(cliniti
    PRIVATE
        ${CURSES_LIBRARIES}
)

include(GNUInstallDirs)

install(TARGETS cliniti
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
