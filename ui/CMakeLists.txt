find_package(Qt5 COMPONENTS Widgets Charts Core REQUIRED)

set(TARGET_NAME ui)
set(TARGET_UI_FILES ui/MainForm.ui)
set(TARGET_SRC_FILES src/MainForm.cpp)
set(TARGET_INCLUDE_DIRS include/)
set(TARGET_MOC_FILES include/MainForm.hpp)

qt_wrap_cpp(TARGET_SRC_FILES ${TARGET_MOC_FILES})
qt_wrap_ui(TARGET_SRC_FILES ${TARGET_UI_FILES})

add_library(${TARGET_NAME} ${TARGET_SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${TARGET_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Widgets Qt5::Charts)