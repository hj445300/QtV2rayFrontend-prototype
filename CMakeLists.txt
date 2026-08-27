cmake_minimum_required(VERSION 3.5)
project(QtV2rayFrontend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

set(SRCS
    src/main.cpp
    src/MainWindow.cpp
    src/ConfigEditorDialog.cpp
    src/ChartWidget.cpp
)

set(HDRS
    src/MainWindow.h
    src/ConfigEditorDialog.h
    src/ChartWidget.h
)

set(RC resources/resources.qrc)

add_executable(${PROJECT_NAME} ${SRCS} ${HDRS} ${RC})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets Qt5::Network)
