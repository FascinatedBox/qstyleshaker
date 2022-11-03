project(qstyleshaker)
cmake_minimum_required(VERSION 3.5.0)

find_package(
    Qt5
    REQUIRED
        Core
)

set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)
set(QSTYLESHAKER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_executable(
    qstyleshaker
        src/qstyleshaker.cpp
)

set_target_properties(
    qstyleshaker
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
            ${QSTYLESHAKER_BINARY_DIR}
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
