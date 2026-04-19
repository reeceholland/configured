/**
 * @file test_project_service.cpp
 * @brief Unit tests for ProjectService load and save workflows.
 *
 * These tests verify application-level project persistence behavior, including
 * loading existing project files, rejecting invalid paths, writing valid projects,
 * and rejecting invalid project data before save.
 */

#include <gtest/gtest.h>

#include <QTemporaryDir>

#include "core/ConfiguredProject.hpp"
#include "core/ProjectService.hpp"

/**
 * @brief Verifies that ProjectService can load a project saved to disk.
 */
TEST(ProjectServiceTest, LoadProjectLoadsExistingProject) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  const QString filePath = dir.filePath("test.configured");

  ConfiguredProject project;
  project.createSampleProject();
  project.setName("Loaded Project");

  QString saveError;
  ASSERT_TRUE(project.saveToFile(filePath, &saveError)) << saveError.toStdString();

  ProjectService service(nullptr);

  QString error;
  auto loaded = service.loadProject(filePath, error);

  ASSERT_NE(loaded, nullptr) << error.toStdString();
  EXPECT_EQ(loaded->name(), "Loaded Project");
}

/**
 * @brief Verifies that ProjectService rejects an empty file path when loading a project.
 */
TEST(ProjectServiceTest, LoadProjectRejectEmptyPath) {
  ProjectService service(nullptr);

  QString error;
  auto loaded = service.loadProject("", error);

  ASSERT_EQ(loaded, nullptr);
  EXPECT_FALSE(error.isEmpty());
}

/**
 * @brief Verifies that ProjectService rejects an empty file path when saving a project.
 */
TEST(ProjectServiceTest, SaveProjectRejectEmptyPath) {
  ProjectService service(nullptr);

  ConfiguredProject project;
  project.createSampleProject();

  QString error;
  EXPECT_FALSE(service.saveProject(project, "", error));
  EXPECT_FALSE(error.isEmpty());
}

/**
 * @brief Verifies that ProjectService can save a project to disk and that the saved file can be
 * loaded back.
 */
TEST(ProjectServiceTest, SaveProjectWritesFile) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  const QString filePath = dir.filePath("test.configured");

  ConfiguredProject project;
  project.createSampleProject();
  project.setName("Saved Project");

  ProjectService service(nullptr);

  QString error;
  EXPECT_TRUE(service.saveProject(project, filePath, error)) << error.toStdString();

  ConfiguredProject loaded;
  ASSERT_TRUE(loaded.loadFromFile(filePath, &error)) << error.toStdString();
  EXPECT_EQ(loaded.name(), "Saved Project");
}

/**
 * @brief Verifies that ProjectService rejects saving a project with duplicate parameter keys.
 */
TEST(ProjectServiceTest, SaveProjectRejectsDuplicateParameterKeys) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  const QString filePath = dir.filePath("duplicate.configured");

  ConfiguredProject project;
  project.createSampleProject();

  auto first = std::make_unique<ConfiguredItem>("ParamOne", ConfiguredItemType::Parameter);
  first->setParameterKey("duplicate_key");
  first->setParameterValue("1");

  auto second = std::make_unique<ConfiguredItem>("ParamTwo", ConfiguredItemType::Parameter);
  second->setParameterKey("duplicate_key");
  second->setParameterValue("2");

  project.root()->addChild(std::move(first));
  project.root()->addChild(std::move(second));

  ProjectService service(nullptr);

  QString error;
  EXPECT_FALSE(service.saveProject(project, filePath, error));
  EXPECT_FALSE(error.isEmpty());
}
