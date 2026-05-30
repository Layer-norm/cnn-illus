cmake_minimum_required(VERSION 3.16)
project(CNN_Visualizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    mainwindow.cpp
    convolutionwidget.cpp
    poolingwidget.cpp
    activationwidget.cpp
    overviewwidget.cpp
    theme.cpp
)

set(HEADERS
    mainwindow.h
    convolutionwidget.h
    poolingwidget.h
    activationwidget.h
    overviewwidget.h
    theme.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
