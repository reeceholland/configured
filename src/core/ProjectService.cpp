#include "core/ProjectService.hpp"

#include <QDir>
#include <QFileInfo>

#include "core/ConfiguredProject.hpp"
#include "core/GitService.hpp"

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
  if (!baseDir.exists(projectName)) {
    if (!baseDir.exists()) {
      result.errorMessage = "Base folder does not exist.";
      return result;
    }
  }

  const QString projectFolderPath = baseDir.filePath(projectName);
  if (!baseDir.exists(projectName) && !baseDir.mkdir(projectName)) {
    result.errorMessage = "Could not create project folder.";
    return result;
  }

  const QString projectFilePath = QDir(projectFolderPath).filePath(projectName + ".configured");

  auto project = std::make_unique<ConfiguredProject>();
  project->createSampleProject();
  project->setName(projectName);
  project->setDescription(metadata.description);
  project->setAuthor(metadata.author);
  project->setCompany(metadata.company);
  project->setVersion(metadata.version);
  project->setRobotPlatform(metadata.robotPlatform);
  project->setGitManaged(metadata.gitManaged);

  if (!project->saveToFile(projectFilePath)) {
    result.errorMessage = "Could not save project file.";
    return result;
  }

  if (metadata.gitManaged) {
    if (!gitService_) {
      result.errorMessage = "Git service not available.";
      return result;
    }

    QString output;
    if (!gitService_->isGitAvailable(&output)) {
      result.errorMessage = "Git is not available: " + output;
      return result;
    }

    if (!gitService_->initRepository(projectFolderPath, &output)) {
      result.errorMessage = "Failed to initialize Git repository: " + output;
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
  project.setName(metadata.name);
  project.setDescription(metadata.description);
  project.setAuthor(metadata.author);
  project.setCompany(metadata.company);
  project.setVersion(metadata.version);
  project.setRobotPlatform(metadata.robotPlatform);

  const bool wasGitManaged = project.isGitManaged();
  project.setGitManaged(metadata.gitManaged);

  const QString repoDir = QFileInfo(projectFilePath).absolutePath();

  if (!wasGitManaged && metadata.gitManaged) {
    if (!gitService_) {
      error = "Git service not available.";
      return false;
    }

    QString output;
    if (!gitService_->isGitAvailable(&output)) {
      error = "Git is not available.\n\n" + output;
      return false;
    }

    if (!gitService_->initRepository(repoDir, &output)) {
      error = "Failed to initialize Git repository.\n\n" + output;
      return false;
    }
  }

  return true;
}