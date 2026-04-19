#pragma once

#include <QString>
#include <memory>

#include "core/ProjectMetadata.hpp"

class ConfiguredProject;
class GitService;

struct ProjectCreationResult {
  bool success = false;
  QString errorMessage;
  QString projectFilePath;
  std::unique_ptr<ConfiguredProject> project;
};

class ProjectService {
 public:
  explicit ProjectService(GitService* gitService);

  ProjectCreationResult createProject(const ProjectMetadata& metadata,
                                      const QString& baseFolder) const;

  bool updateProjectMetadata(ConfiguredProject& project, const ProjectMetadata& metadata,
                             const QString& projectFilePath, QString& error) const;

  bool ensureGitInitialized(const QString& repoDir, QString& error) const;

  bool saveProject(ConfiguredProject& project, const QString& projectFilePath,
                   QString& error) const;

  std::unique_ptr<ConfiguredProject> loadProject(const QString& projectFilePath,
                                                 QString& error) const;

 private:
  GitService* gitService_ = nullptr;
};