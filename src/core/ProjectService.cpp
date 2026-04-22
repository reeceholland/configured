#include "core/ProjectService.hpp"

#include <QDir>
#include <QFileInfo>

#include "core/ConfiguredProject.hpp"
#include "core/git/GitService.hpp"
#include "core/ProjectMetadataService.hpp"
#include "core/validation/metadata/ProjectMetadataValidator.hpp"
#include "core/validation/project/ProjectValidator.hpp"

ProjectService::ProjectService(GitService* gitService) : gitService_(gitService) {}

ProjectCreationResult ProjectService::createProject(const ProjectMetadata& metadata,
                                                    const QString& baseFolder) const {
  ProjectCreationResult result;

  const QString projectName = metadata.name.trimmed();
  if (projectName.isEmpty()) {
    result.errorMessage = "Project name cannot be empty.";
    return result;
  }

  QDir baseDir(baseFolder);
  if (!baseDir.exists()) {
    result.errorMessage = "Base folder does not exist.";
    return result;
  }

  const QString projectFolderPath = baseDir.filePath(projectName);
  if (!baseDir.exists(projectName) && !baseDir.mkdir(projectName)) {
    result.errorMessage = "Could not create project folder.";
    return result;
  }

  const QString projectFilePath = QDir(projectFolderPath).filePath(projectName + ".configured");

  auto project = std::make_unique<ConfiguredProject>();
  project->createSampleProject();

  ProjectMetadataValidator validator;
  const ValidationResult validationResult = validator.validate(metadata);

  if (!validationResult.isValid()) {
    result.errorMessage = validationResult.messages().first().message;
    return result;
  }

  ProjectMetadataService::apply(*project, metadata);

  if (!project->saveToFile(projectFilePath)) {
    result.errorMessage = "Could not save project file.";
    return result;
  }

  if (metadata.gitManaged) {
    if (!ensureGitInitialized(projectFolderPath, result.errorMessage)) {
      return result;
    }
  }

  result.success = true;
  result.projectFilePath = projectFilePath;
  result.project = std::move(project);
  return result;
}

#include <QFileInfo>

bool ProjectService::updateProjectMetadata(ConfiguredProject& project,
                                           const ProjectMetadata& metadata,
                                           const QString& projectFilePath, QString& error) const {
  ProjectMetadataValidator validator;
  const ValidationResult validationResult = validator.validate(metadata);

  if (!validationResult.isValid()) {
    error = validationResult.messages().first().message;
    return false;
  }

  const bool wasGitManaged = project.isGitManaged();

  ProjectMetadataService::apply(project, metadata);

  const QString repoDir = QFileInfo(projectFilePath).absolutePath();

  if (!wasGitManaged && metadata.gitManaged) {
    if (!ensureGitInitialized(repoDir, error)) {
      return false;
    }
  }

  return true;
}

bool ProjectService::ensureGitInitialized(const QString& repoDir, QString& error) const {
  if (!gitService_) {
    error = "Git service not available.";
    return false;
  }

  QString output;

  if (!gitService_->isGitAvailable(&output)) {
    error = "Git is not available: " + output;
    return false;
  }

  if (!gitService_->initRepository(repoDir, &output)) {
    error = "Failed to initialize Git repository: " + output;
    return false;
  }

  return true;
}

bool ProjectService::saveProject(ConfiguredProject& project, const QString& projectFilePath,
                                 QString& error) const {
  error.clear();

  if (projectFilePath.trimmed().isEmpty()) {
    error = "Project file path is empty.";
    return false;
  }

  const QFileInfo fileInfo(projectFilePath);
  if (!fileInfo.absoluteDir().exists()) {
    error = "Project folder does not exist.";
    return false;
  }

  const ValidationResult validationResult = ProjectValidator().validate(project);
  if (!validationResult.isValid()) {
    error = validationResult.messages().first().message;
    return false;
  }

  if (!project.saveToFile(projectFilePath, &error)) {
    error = error.isEmpty() ? "Could not save project file." : error;
    return false;
  }

  return true;
}

std::unique_ptr<ConfiguredProject> ProjectService::loadProject(const QString& projectFilePath,
                                                               QString& error) const {
  error.clear();

  if (projectFilePath.trimmed().isEmpty()) {
    error = "Project file path is empty.";
    return nullptr;
  }

  auto project = std::make_unique<ConfiguredProject>();

  if (!project->loadFromFile(projectFilePath)) {
    error = "Could not load project file.";
    return nullptr;
  }

  return project;
}
