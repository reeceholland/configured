cmake_minimum_required(VERSION 3.21)
project(Configured LANGUAGES CXX VERSION 0.4.0)

add_compile_definitions(APP_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

add_executable(Configured
    src/main.cpp

    src/app/AppLogger.cpp
    include/app/AppLogger.hpp

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

    src/core/git/GitService.cpp
    include/core/git/GitService.hpp

    src/ui/GitConfigDialog.cpp
    include/ui/GitConfigDialog.hpp

    src/ui/HelpScreenWidget.cpp
    include/ui/HelpScreenWidget.hpp

    src/export/XmlProjectExporter.cpp
    include/export/XmlProjectExporter.hpp

    src/export/JsonProjectExporter.cpp
    include/export/JsonProjectExporter.hpp

    src/core/ProjectService.cpp
    include/core/ProjectService.hpp

    src/core/ProjectMetadataService.cpp
    include/core/ProjectMetadataService.hpp

    src/ui/RemoteConnectDialog.cpp
    include/ui/RemoteConnectDialog.hpp

    src/ui/ConnectingDialog.cpp
    include/ui/ConnectingDialog.hpp

    src/core/git/CloneWorker.cpp
    include/core/git/CloneWorker.hpp

    src/core/git/GitPullWorker.cpp
    include/core/git/GitPullWorker.hpp

    src/ui/PullingDialog.cpp
    include/ui/PullingDialog.hpp

    src/core/validation/ValidationResult.cpp
    src/core/validation/metadata/ProjectMetadataValidator.cpp
    src/core/validation/metadata/ProjectMetadataValidators.cpp
    src/core/validation/item/ItemValidator.cpp
    src/core/validation/item/ItemValidators.cpp
    src/core/validation/project/ProjectValidator.cpp

    resources/app.qrc
)

target_include_directories(Configured PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(Configured PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

include(CTest)
enable_testing()

add_subdirectory(tests)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found, 'doc' target will not be available")
endif()

install(TARGETS Configured
    RUNTIME DESTINATION .
)
