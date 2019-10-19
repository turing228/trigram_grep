cmake_minimum_required(VERSION 2.8)

project(trigram_grep)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

add_executable(trigram_grep main.cpp mainwindow.h mainwindow.cpp back/Trigram.cpp back/headers/Trigram.h back/IndexingWorker.cpp back/headers/IndexingWorker.h back/SearchingWorker.cpp back/headers/SearchingWorker.h)
qt5_use_modules(trigram_grep Widgets)
