set(TARGET_NAME benchmark)
set(TARGET_INCLUDE_DIRS include/)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE ${TARGET_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} INTERFACE arr btree)
