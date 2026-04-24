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
    test_configured_item.cpp
    test_configured_project.cpp
    test_validation.cpp
    test_project_service.cpp
    test_project_validator.cpp
    test_git_service.cpp

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
)

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
)

include(GoogleTest)
gtest_discover_tests(ConfiguredTests)
