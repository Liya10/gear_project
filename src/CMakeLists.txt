cmake_minimum_required(VERSION 3.5)

project (gear_project)




set(SOURCES
       geometry.cpp
       io.cpp
       main.cpp
)
set(HEADERS
       geometry.h
       io.h
       matrix.h
       matrix.hpp
)
add_executable(main ${SOURCES} ${HEADERS})

set_target_properties(
    main PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
     main
     PRIVATE
     ${CMAKE_CURRENT_BINARY_DIR}

)
target_include_directories(
     main
#     PRIVATE
     PUBLIC
     ${CMAKE_CURRENT_SOURCE_DIR}/../lib

)
target_link_libraries(main LINK_PUBLIC lib)