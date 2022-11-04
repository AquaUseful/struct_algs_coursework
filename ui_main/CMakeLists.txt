set(TARGET_NAME ui_main)
set(TARGET_SRC_FILES src/ui_main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SRC_FILES})
target_link_libraries(${TARGET_NAME} ui)
