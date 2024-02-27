project(qstyleshaker)
cmake_minimum_required(VERSION 3.5.0)

find_package(
    Qt5
    COMPONENTS
        Core
    REQUIRED
)

set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

add_executable(
    qstyleshaker
        src/qstyleshaker.cpp
)

target_link_libraries(
    qstyleshaker
        Qt5::Core
)

install(
    TARGETS
        qstyleshaker
    DESTINATION
        bin
)
