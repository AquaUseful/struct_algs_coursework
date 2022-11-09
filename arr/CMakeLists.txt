set(TARGET_NAME arr)
set(TARGET_INCLUDE_DIRS include/)

add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE ${TARGET_INCLUDE_DIRS})
