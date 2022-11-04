cmake_minimum_required(VERSION 3.20)

project(stralg_coursework VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

add_subdirectory(ui)
add_subdirectory(btree)
add_subdirectory(main)
add_subdirectory(ui_main)


#add_subdirectory(doctest)
