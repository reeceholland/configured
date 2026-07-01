/**
 * @file test_project_service.cpp
 * @brief Unit tests for ProjectService load and save workflows.
 *
 * These tests verify application-level project persistence behavior, including
 * loading existing project files, rejecting invalid paths, writing valid projects,
 * and rejecting invalid project data before save.
 */

#include <gtest/gtest.h>

#include <QDir>
#include <QFileInfo>
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

  auto loaded = service.loadProject(filePath);

  ASSERT_TRUE(loaded) << loaded.error().toStdString();
  EXPECT_EQ(loaded.value()->name(), "Loaded Project");
}

/**
 * @brief Verifies that ProjectService rejects an empty file path when loading a project.
 */
TEST(ProjectServiceTest, LoadProjectRejectEmptyPath) {
  ProjectService service(nullptr);

  auto loaded = service.loadProject("");

  ASSERT_FALSE(loaded);
  EXPECT_FALSE(loaded.error().isEmpty());
}

/**
 * @brief Verifies that ProjectService rejects an empty file path when saving a project.
 */
TEST(ProjectServiceTest, SaveProjectRejectEmptyPath) {
  ProjectService service(nullptr);

  ConfiguredProject project;
  project.createSampleProject();

  auto result = service.saveProject(project, "");
  EXPECT_FALSE(result);
  EXPECT_FALSE(result.error().isEmpty());
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

  auto saveResult = service.saveProject(project, filePath);
  ASSERT_TRUE(saveResult) << saveResult.error().toStdString();

  ConfiguredProject loaded;
  QString error;
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

  auto saveResult = service.saveProject(project, filePath);
  ASSERT_FALSE(saveResult);
  EXPECT_FALSE(saveResult.error().isEmpty());
}

TEST(ProjectServiceTest, CreateProjectRejectsInvalidMetadataName) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  ProjectMetadata metadata;
  metadata.name = "bad/name";
  metadata.gitManaged = false;

  ProjectService service(nullptr);
  const ProjectCreationResult result = service.createProject(metadata, dir.path());

  EXPECT_FALSE(result.success);
  EXPECT_EQ(result.project, nullptr);
  EXPECT_FALSE(result.errorMessage.isEmpty());
}

TEST(ProjectServiceTest, CreateProjectWritesConfiguredFile) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  ProjectMetadata metadata;
  metadata.name = "ReleaseReady";
  metadata.author = "Reece";
  metadata.version = "1.0.0";
  metadata.gitManaged = false;

  ProjectService service(nullptr);
  const ProjectCreationResult result = service.createProject(metadata, dir.path());

  ASSERT_TRUE(result.success) << result.errorMessage.toStdString();
  ASSERT_NE(result.project, nullptr);
  EXPECT_EQ(result.project->name(), "ReleaseReady");
  EXPECT_TRUE(QFileInfo::exists(result.projectFilePath));
  EXPECT_TRUE(result.projectFilePath.endsWith("ReleaseReady.configured"));
}

TEST(ProjectServiceTest, SaveProjectAsWritesProjectInOwnFolderAndReturnsPath) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  ConfiguredProject project;
  project.createSampleProject();
  project.setName("CopiedProject");

  ProjectService service(nullptr);
  const ProjectSaveAsResult result = service.saveProjectAs(project, dir.path());

  const QString expectedFolder = QDir(dir.path()).filePath("CopiedProject");
  const QString expectedFile = QDir(expectedFolder).filePath("CopiedProject.configured");

  ASSERT_TRUE(result.success) << result.errorMessage.toStdString();
  EXPECT_EQ(result.projectFilePath, expectedFile);
  EXPECT_TRUE(QFileInfo::exists(expectedFile));
}

TEST(ProjectServiceTest, SaveProjectAsCreatesStandaloneNonGitCopy) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  ConfiguredProject project;
  project.createSampleProject();
  project.setName("StandaloneCopy");
  project.setGitManaged(true);
  project.setGitCommitHash("abc123");

  ProjectService service(nullptr);
  const ProjectSaveAsResult result = service.saveProjectAs(project, dir.path());

  ASSERT_TRUE(result.success) << result.errorMessage.toStdString();
  EXPECT_FALSE(project.isGitManaged());
  EXPECT_TRUE(project.gitCommitHash().isEmpty());

  ConfiguredProject loaded;
  QString error;
  ASSERT_TRUE(loaded.loadFromFile(result.projectFilePath, &error)) << error.toStdString();
  EXPECT_FALSE(loaded.isGitManaged());
}
