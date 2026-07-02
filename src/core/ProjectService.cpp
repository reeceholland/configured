#include "core/ProjectService.hpp"

#include <QDir>
#include <QFileInfo>
#include <QtCore/QLoggingCategory>

#include "core/ConfiguredProject.hpp"
#include "core/ProjectMetadataService.hpp"
#include "core/git/GitService.hpp"
#include "core/validation/metadata/ProjectMetadataValidator.hpp"
#include "core/validation/project/ProjectValidator.hpp"

Q_LOGGING_CATEGORY(logProjectService, "configured.core.projectservice")

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
    if (auto success = ensureGitInitialized(projectFolderPath); !success) {
      result.errorMessage =
          success.error().isEmpty() ? "Failed to initialize Git repository." : success.error();
      return result;
    }
  }

  result.success = true;
  result.projectFilePath = projectFilePath;
  result.project = std::move(project);
  return result;
}

std::expected<void, QString> ProjectService::updateProjectMetadata(
    ConfiguredProject& project, const ProjectMetadata& metadata,
    const QString& projectFilePath) const {
  ProjectMetadataValidator validator;
  const ValidationResult validationResult = validator.validate(metadata);

  if (!validationResult.isValid()) {
    return std::unexpected(validationResult.messages().first().message);
  }

  const bool wasGitManaged = project.isGitManaged();

  ProjectMetadataService::apply(project, metadata);

  const QString repoDir = QFileInfo(projectFilePath).absolutePath();

  if (!wasGitManaged && metadata.gitManaged) {
    if (auto success = ensureGitInitialized(repoDir); !success) {
      return std::unexpected(success.error().isEmpty() ? "Failed to initialize Git repository."
                                                       : success.error());
    }
  }

  return {};
}

std::expected<void, QString> ProjectService::ensureGitInitialized(const QString& repoDir) const {
  if (!gitService_) {
    return std::unexpected("Git service not available.");
  }

  QString output;

  if (!gitService_->isGitAvailable(&output)) {
    return std::unexpected("Git is not available: " + output);
  }

  if (!gitService_->initRepository(repoDir, &output)) {
    return std::unexpected("Failed to initialize Git repository: " + output);
  }

  return {};
}

std::expected<void, QString> ProjectService::saveProject(ConfiguredProject& project,
                                                         const QString& projectFilePath) const {
  if (projectFilePath.trimmed().isEmpty()) {
    return std::unexpected(QStringLiteral("Project file path is empty."));
  }

  const QFileInfo fileInfo(projectFilePath);
  if (!fileInfo.absoluteDir().exists()) {
    return std::unexpected(QStringLiteral("Project folder does not exist."));
  }

  const ValidationResult validationResult = ProjectValidator().validate(project);
  if (!validationResult.isValid()) {
    return std::unexpected(validationResult.messages().first().message);
  }

  QString error;
  if (!project.saveToFile(projectFilePath, &error)) {
    return std::unexpected(error.isEmpty() ? "Could not save project file." : error);
  }

  return {};
}

std::expected<std::unique_ptr<ConfiguredProject>, QString> ProjectService::loadProject(
    const QString& projectFilePath) const {
  if (projectFilePath.trimmed().isEmpty()) {
    return std::unexpected(QStringLiteral("Project file path is empty."));
  }

  auto project = std::make_unique<ConfiguredProject>();

  QString loadError;
  if (!project->loadFromFile(projectFilePath, &loadError)) {
    qCCritical(logProjectService) << "Failed to load project from" << projectFilePath << ":"
                                  << loadError;
    QString message = QStringLiteral("Could not load project file.");
    if (!loadError.isEmpty()) {
      message += QStringLiteral(" ");
      message += loadError;
    }
    return std::unexpected(message);
  }

  return std::move(project);
}

ProjectSaveAsResult ProjectService::saveProjectAs(ConfiguredProject& project,
                                                  const QString& baseDirectory) const {
  ProjectSaveAsResult result;

  const QString projectName = project.name().trimmed();
  if (projectName.isEmpty()) {
    result.errorMessage = "Project name cannot be empty.";
    return result;
  }

  const ValidationResult validationResult = ProjectValidator().validate(project);
  if (!validationResult.isValid()) {
    result.errorMessage = validationResult.messages().first().message;
    return result;
  }

  QDir baseDir(baseDirectory);
  if (!baseDir.exists()) {
    result.errorMessage = "Base directory does not exist.";
    return result;
  }

  const QString projectFolderPath = baseDir.filePath(projectName);
  if (!baseDir.exists(projectName) && !baseDir.mkdir(projectName)) {
    result.errorMessage = "Could not create project folder.";
    return result;
  }

  const QString projectFilePath = QDir(projectFolderPath).filePath(projectName + ".configured");
  const bool originalGitManaged = project.isGitManaged();
  const QString originalGitCommitHash = project.gitCommitHash();

  project.setGitManaged(false);
  project.setGitCommitHash("");

  auto saveResult = saveProject(project, projectFilePath);
  if (!saveResult) {
    project.setGitManaged(originalGitManaged);
    project.setGitCommitHash(originalGitCommitHash);
    result.errorMessage =
        saveResult.error().isEmpty() ? "Could not save project file." : saveResult.error();
    return result;
  }

  result.success = true;
  result.projectFilePath = projectFilePath;
  return result;
}
