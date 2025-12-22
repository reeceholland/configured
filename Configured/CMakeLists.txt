cmake_minimum_required(VERSION 3.24)

project(Configured LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
qt_standard_project_setup()

qt_add_executable(Configured
  src/main.cpp

  # App shell
  src/app/MainWindow.hpp
  src/app/MainWindow.cpp

  src/ui/HomeScreenWidget.hpp
  src/ui/HomeScreenWidget.cpp
  src/ui/BottomTextButton.hpp
  src/ui/BottomTextButton.cpp
  src/ui/ConnectWindow.hpp
  src/ui/ConnectWindow.cpp
  src/ui/ConnectionType.hpp

)

qt_add_resources(Configured app_icons
  FILES
  resources/icons/usb_connection_96.png
  resources/icons/add_plus_circle_128.png
  resources/icons/settings_cog_128.png
  resources/icons/question_mark_circle_128.png
)

target_link_libraries(Configured PRIVATE Qt6::Widgets)

target_include_directories(Configured PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/src/core
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

target_link_libraries(Configured PRIVATE
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::Sql
)

set_target_properties(Configured PROPERTIES
  WIN32_EXECUTABLE ON
)
