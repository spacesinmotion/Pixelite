cmake_minimum_required(VERSION 3.1.0)

project(Pixelite VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(Pixelite
  PixeliteMain.cpp
  Pixelite.cpp
  Pixelite.h
  Pixelite.ui
  Pixelite.qrc
  NewImageDialog.cpp
  NewImageDialog.h
  NewImageDialog.ui
  DrawPane.cpp
  DrawPane.h
  ColorPalette.cpp
  ColorPalette.h
  ColorSelector.cpp
  ColorSelector.h
)

target_include_directories(Pixelite PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Pixelite Qt5::Widgets)