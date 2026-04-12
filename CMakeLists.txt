cmake_minimum_required(VERSION 3.21)
project(Configured LANGUAGES CXX VERSION 0.1.0)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(Configured
    src/main.cpp

    src/app/MainWindow.cpp
    include/app/MainWindow.hpp

    src/ui/HomeScreenWidget.cpp
    include/ui/HomeScreenWidget.hpp

    src/ui/EditorScreenWidget.cpp
    include/ui/EditorScreenWidget.hpp

    src/core/ConfiguredItem.cpp
    include/core/ConfiguredItem.hpp

    src/core/ConfiguredProject.cpp
    include/core/ConfiguredProject.hpp

    src/ui/ProjectMetadataDialog.cpp
    include/ui/ProjectMetadataDialog.hpp

    src/core/GitService.cpp
    include/core/GitService.hpp

    src/ui/GitConfigDialog.cpp
    include/ui/GitConfigDialog.hpp

    src/ui/HelpScreenWidget.cpp
    include/ui/HelpScreenWidget.hpp

    resources/app.qrc
)

target_include_directories(Configured PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Configured PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

include(CTest)
enable_testing()

add_subdirectory(tests)