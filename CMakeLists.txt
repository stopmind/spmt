cmake_minimum_required(VERSION 3.25)
project(spmt)

set(CMAKE_CXX_STANDARD 17)

add_executable(spmt Package.hpp Package.cpp Version.hpp Version.cpp Args.hpp Args.cpp Data.hpp Data.cpp Defines.hpp main.cpp Config.hpp Config.cpp)


if(NOT DEFINED CONF_DIR)
    set(CONF_DIR /etc)
endif()

if(NOT DEFINED TEMP_DIR)
    set(TEMP_DIR /tmp)
endif()

if(NOT DEFINED DATA_DIR)
    set(DATA_DIR /var)
endif()

if(NOT DEFINED INSTALL_PREFIX)
    set(INSTALL_PREFIX /)
endif()

message("CONF_DIR       : ${CONF_DIR}")
message("TEMP_DIR       : ${TEMP_DIR}")
message("DATA_DIR       : ${DATA_DIR}")
message("INSTALL_PREFIX : ${INSTALL_PREFIX}")

set(CMAKE_INSTALL_PREFIX  ${INSTALL_PREFIX})

add_compile_definitions(CONF_DIR=\"${CONF_DIR}\")
add_compile_definitions(TEMP_DIR=\"${TEMP_DIR}\")
add_compile_definitions(DATA_DIR=\"${DATA_DIR}\")

install(TARGETS spmt)