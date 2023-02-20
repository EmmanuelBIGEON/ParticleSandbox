cmake_minimum_required(VERSION 3.18)

project(ParticleSandbox)

set(SOURCES
    ../src/main.cpp
)

add_executable(ParticleSandbox ${SOURCES})

install(TARGETS ParticleSandbox DESTINATION ${CMAKE_INSTALL_PREFIX})