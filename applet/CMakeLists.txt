cmake_minimum_required(VERSION 3.16)

project(WKDockerApp LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui DBus)
INCLUDE_DIRECTORIES(${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS})

add_executable(WKDockerApp
  main.cpp
  dockerdaemon.h
  dockerdaemon.cpp
  constants.h
  constants.cpp
  trayitem.h
  trayitem.cpp
  configsettings.h
  configsettings.cpp
  resources/resources.qrc
)
target_link_libraries(WKDockerApp
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::DBus)

include(GNUInstallDirs)
install(TARGETS WKDockerApp
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
