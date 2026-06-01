#pragma once

#include <QString>
#include <memory>

#include "core/ProjectMetadata.hpp"

class ConfiguredProject;
class GitService;

/**
 * @brief Result returned after creating a project on disk.
 */
struct ProjectCreationResult {
  /// True when the project folder, project file, and optional Git setup succeeded.
  bool success = false;

  /// User-facing failure details when success is false.
  QString errorMessage;

  /// Full path to the created .configured project file.
  QString projectFilePath;

  /// Newly created project instance ready to show in the editor.
  std::unique_ptr<ConfiguredProject> project;
};

/**
 * @brief Result returned after saving a copy of the active project.
 */
struct ProjectSaveAsResult {
  /// True when the copy was written successfully.
  bool success = false;

  /// User-facing failure details when success is false.
  QString errorMessage;

  /// Full path to the copied .configured project file.
  QString projectFilePath;
};

/**
 * @brief Coordinates project file operations and optional Git repository setup.
 * ProjectService is the application-level boundary around creating, loading, saving, and updating
 * ConfiguredProject instances. It delegates Git-specific work to GitService when a project is
 * marked as Git-managed.
 */
class ProjectService {
 public:
  /**
   * @brief Create a service.
   * @param gitService Optional Git service used for repository initialization.
   */
  explicit ProjectService(GitService* gitService);

  /**
   * @brief Create a new project folder and .configured file.
   * @param metadata Metadata to apply to the new project.
   * @param baseFolder Parent folder where the project folder will be created.
   * @return Creation result containing either the project or an error message.
   */
  ProjectCreationResult createProject(const ProjectMetadata& metadata,
                                      const QString& baseFolder) const;

  /**
   * @brief Apply metadata changes and save the project file.
   * @param project Project to update.
   * @param metadata New metadata values.
   * @param projectFilePath Existing project file path.
   * @param error User-facing error populated on failure.
   * @return true when metadata was applied and saved.
   */
  bool updateProjectMetadata(ConfiguredProject& project, const ProjectMetadata& metadata,
                             const QString& projectFilePath, QString& error) const;

  /**
   * @brief Ensure a directory is initialized as a Git repository.
   * @param repoDir Working directory to initialize.
   * @param error User-facing error populated on failure.
   * @return true when the directory is already a repository or was initialized.
   */
  bool ensureGitInitialized(const QString& repoDir, QString& error) const;

  /**
   * @brief Save an existing project to disk.
   * @param project Project model to persist.
   * @param projectFilePath Destination .configured file path.
   * @param error User-facing error populated on failure.
   * @return true when the project file was written.
   */
  bool saveProject(ConfiguredProject& project, const QString& projectFilePath,
                   QString& error) const;

  /**
   * @brief Load a project from disk.
   * @param projectFilePath Source .configured file path.
   * @param error User-facing error populated on failure.
   * @return Loaded project, or nullptr on failure.
   */
  std::unique_ptr<ConfiguredProject> loadProject(const QString& projectFilePath,
                                                 QString& error) const;

  /**
   * @brief Save a standalone copy of a project inside its own project folder.
   * @param project Project model to copy.
   * @param baseDirectory Parent folder where the project folder will be created.
   * @return Save As result containing either the copied file path or an error message.
   */
  ProjectSaveAsResult saveProjectAs(ConfiguredProject& project, const QString& baseDirectory) const;

 private:
  GitService* gitService_ = nullptr;
};
