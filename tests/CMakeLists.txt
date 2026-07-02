cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Prevent overriding parent project settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(ConfiguredTests
    test_main.cpp
    test_configured_item.cpp
    test_configured_project.cpp
    test_validation.cpp
    test_project_service.cpp
    test_project_validator.cpp
    test_project_metadata_validator.cpp
    test_git_service.cpp
    test_git_workflow_controller.cpp
    test_status_bar_controller.cpp

    ../src/core/ConfiguredItem.cpp
    ../src/core/ConfiguredProject.cpp
    ../src/core/ProjectService.cpp
    ../src/core/ProjectMetadataService.cpp
    ../src/core/git/GitService.cpp
    ../src/core/validation/ValidationResult.cpp
    ../src/core/validation/metadata/ProjectMetadataValidator.cpp
    ../src/core/validation/metadata/ProjectMetadataValidators.cpp
    ../src/core/validation/item/ItemValidator.cpp
    ../src/core/validation/item/ItemValidators.cpp
    ../src/core/validation/project/ProjectValidator.cpp
    
    ../src/app/GitWorkflowController.cpp
    ../include/app/GitWorkflowController.hpp

    ../src/core/git/GitPullWorker.cpp
    ../include/core/git/GitPullWorker.hpp

    ../src/core/git/GitPushWorker.cpp
    ../include/core/git/GitPushWorker.hpp

    ../src/ui/PullPreflightDialog.cpp
    ../include/ui/PullPreflightDialog.hpp

    ../src/ui/PullingDialog.cpp
    ../include/ui/PullingDialog.hpp

    ../src/ui/PushingDialog.cpp
    ../include/ui/PushingDialog.hpp

    ../src/ui/NewProjectGitWizard.cpp
    ../include/ui/NewProjectGitWizard.hpp

    ../src/app/StatusBarController.cpp
    ../include/app/StatusBarController.hpp

    ../src/core/ConfiguredItemTypes.cpp
    ../include/core/ConfiguredItemTypes.hpp
)

target_compile_features(ConfiguredTests PRIVATE cxx_std_23)

if (MSVC)
    target_compile_options(ConfiguredTests PRIVATE /EHsc)
endif()

target_include_directories(ConfiguredTests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ConfiguredTests PRIVATE
    GTest::gtest
    GTest::gtest_main
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

include(GoogleTest)
gtest_discover_tests(ConfiguredTests)
