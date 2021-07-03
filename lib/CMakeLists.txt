cmake_minimum_required(VERSION 3.5)

project (gear_project)


set(SOURCES
       EasyBMP.cpp
)
set(HEADERS
       EasyBMP.h
       EasyBMP_BMP.h
       EasyBMP_DataStructures.h
       EasyBMP_VariousBMPutilities.h
)
add_library(lib ${SOURCES} ${HEADERS})

set_target_properties(
    lib PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
     lib
     PUBLIC
     ${CMAKE_CURRENT_BINARY_DIR}

)