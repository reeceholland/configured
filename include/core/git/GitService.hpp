/**
 * @file GitService.hpp
 * @author Reece Holland
 * @brief Git service for the Configured application.
 * @version 0.2
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QString>
#include <QStringList>

/**
 * @brief Git service for interacting with Git repositories.
 *
 * The GitService class provides methods to perform common Git operations such as checking if Git is
 * available, checking if a directory is a Git repository, initializing a repository, checking
 * status, adding files, committing changes, managing remotes, and checking for unpushed commits.
 */
class GitService {
 public:
  /**
   * @brief Check if Git is available on the system.
   *
   * This method checks if the Git executable is available and can be executed.
   *
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if Git is available, false otherwise.
   */
  bool isGitAvailable(QString* output = nullptr) const;

  /**
   * @brief Check if the specified directory is a Git repository.
   *
   * This method checks if the given working directory is part of a Git repository.
   *
   * @param workingDir The path to the working directory to check.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the directory is a Git repository, false otherwise.
   */
  bool isRepository(const QString& workingDir, QString* output = nullptr) const;

  /**
   * @brief Initialize a new Git repository in the specified directory.
   *
   * This method initializes a new Git repository in the given working directory.
   *
   * @param workingDir The path to the working directory where the repository should be initialized.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the repository was successfully initialized, false otherwise.
   */
  bool initRepository(const QString& workingDir, QString* output = nullptr) const;

  /**
   * @brief Get the Git status of the specified repository.
   *
   * This method retrieves the status of the Git repository in the given working directory.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param output Optional pointer to a QString to receive the output message, including the
   * status.
   * @return true if the status was successfully retrieved, false otherwise.
   */
  bool status(const QString& workingDir, QString* output = nullptr) const;

  /**
   * @brief Add all changes in the specified repository to the staging area.
   *
   * This method stages all changes (new, modified, and deleted files) in the Git repository at the
   * given working directory.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the changes were successfully staged, false otherwise.
   */
  bool addAll(const QString& workingDir, QString* output = nullptr) const;

  /**
   * @brief Commit staged changes in the specified repository with a commit message.
   *
   * This method creates a new Git commit with the currently staged changes in the repository at
   * the given working directory, using the provided commit message.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param message The commit message to use for the new commit.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the commit was successfully created, false otherwise.
   */
  bool commit(const QString& workingDir, const QString& message, QString* output = nullptr) const;

  /**
   * @brief Get a Git configuration value.
   *
   * This method retrieves the value of a Git configuration key for the specified repository or
   * globally.
   *
   * @param workingDir The path to the working directory of the repository (ignored if global is
   * true).
   * @param key The Git configuration key to retrieve.
   * @param global If true, retrieves the global configuration value; otherwise, retrieves the
   * repository-specific value.
   * @param value Optional pointer to a QString to receive the retrieved configuration value.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the configuration value was successfully retrieved, false otherwise.
   */
  bool getConfigValue(const QString& workingDir, const QString& key, bool global, QString* value,
                      QString* output = nullptr) const;

  /**
   * @brief Set a Git configuration value.
   *
   * This method sets the value of a Git configuration key for the specified repository or globally.
   *
   * @param workingDir The path to the working directory of the repository (ignored if global is
   * true).
   * @param key The Git configuration key to set.
   * @param value The value to set for the configuration key.
   *
   * @param global If true, sets the global configuration value; otherwise, sets the
   * repository-specific value.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the configuration value was successfully set, false otherwise.
   */
  bool setConfigValue(const QString& workingDir, const QString& key, const QString& value,
                      bool global, QString* output = nullptr) const;

  /**
   * @brief Get the Commit Hash object
   *
   * @param workingDir The path to the working directory of the repository.
   * @param hash Optional pointer to a QString to receive the commit hash.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the commit hash was successfully retrieved, false otherwise.
   */
  bool getCommitHash(const QString& workingDir, QString* hash, QString* output = nullptr) const;

  /**
   * @brief Clone a Git repository.
   *
   * This method clones a Git repository from the specified remote URL into the specified parent
   * folder.
   *
   * @param remoteUrl The URL of the remote Git repository to clone.
   * @param parentFolder The path to the parent folder where the repository should be cloned.
   * @param clonedPath Optional pointer to a QString to receive the path of the cloned repository.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the repository was successfully cloned, false otherwise.
   */
  bool cloneRepository(const QString& remoteUrl, const QString& parentFolder, QString* clonedPath,
                       QString* output = nullptr) const;

  /**
   * @brief Check if a remote exists in the repository.
   *
   * This method checks if a remote with the specified name exists in the repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param name The name of the remote to check.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the remote exists, false otherwise.
   */
  bool remoteExists(const QString& workingDir, const QString& name,
                    QString* output = nullptr) const;

  /**
   * @brief Add a remote to the repository.
   *
   * This method adds a remote with the specified name and URL to the repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param name The name of the remote to add.
   * @param url The URL of the remote to add.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the remote was successfully added, false otherwise.
   */
  bool addRemote(const QString& workingDir, const QString& name, const QString& url,
                 QString* output = nullptr) const;

  /**
   * @brief Set the URL of a remote in the repository.
   *
   * This method sets the URL of a remote with the specified name in the repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param name The name of the remote to set the URL for.
   * @param url The new URL of the remote.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the remote URL was successfully set, false otherwise.
   */
  bool setRemoteUrl(const QString& workingDir, const QString& name, const QString& url,
                    QString* output = nullptr) const;

  /**
   * @brief Get the URL of a remote in the repository.
   *
   * This method retrieves the URL of a remote with the specified name in the repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param name The name of the remote to get the URL for.
   * @param url Optional pointer to a QString to receive the URL of the remote.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the URL was successfully retrieved, false otherwise.
   */
  bool remoteUrl(const QString& workingDir, const QString& name, QString* url,
                 QString* output = nullptr) const;

  /**
   * @brief Connect a remote to the repository.
   *
   * This method connects a remote with the specified name and URL to the repository. If the remote
   * already exists, it updates the URL; otherwise, it adds a new remote.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param name The name of the remote to connect.
   * @param url The URL of the remote to connect.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the remote was successfully connected, false otherwise.
   */
  bool connectRemote(const QString& workingDir, const QString& name, const QString& url,
                     QString* output = nullptr) const;

  /**
   * @brief Get the current branch of the repository.
   *
   * This method retrieves the name of the current branch in the repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param branchName Optional pointer to a QString to receive the name of the current branch.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the current branch was successfully retrieved, false otherwise.
   */
  bool currentBranch(const QString& workingDir, QString* branchName,
                     QString* output = nullptr) const;

  /**
   * @brief Check if the working tree is clean.
   *
   * This method checks if the working tree of the repository is clean, meaning there are no
   * uncommitted changes.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param clean Pointer to a bool to receive the result. True if the working tree is clean, false
   * otherwise.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the working tree status was successfully retrieved, false otherwise.
   */
  bool workingTreeClean(const QString& workingDir, bool* clean, QString* output = nullptr) const;

  /**
   * @brief Check if there are unpushed commits in the repository.
   *
   * This method checks if there are commits in the repository that have not been pushed to a remote
   * repository.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param hasUnpushedCommits Pointer to a bool to receive the result. True if there are unpushed
   * commits, false otherwise.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the unpushed commits status was successfully retrieved, false otherwise.
   */
  bool hasUnpushedCommits(const QString& workingDir, bool* hasUnpushedCommits,
                          QString* output = nullptr) const;

  /**
   * @brief Check if the repository has an upstream branch configured.
   *
   * This method checks if the current branch of the repository has an upstream branch configured
   * for pushing and pulling.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param hasUpstream Pointer to a bool to receive the result. True if there
   * is an upstream branch configured, false otherwise.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the upstream status was successfully retrieved, false otherwise.
   */
  bool hasUpstream(const QString& workingDir, bool* hasUpstream, QString* output = nullptr) const;

  bool fetch(const QString& workingDir, QString* output = nullptr) const;

 private:
  /**
   * @brief Run a Git command in the specified working directory.
   *
   * This method runs a Git command with the specified arguments in the given working directory.
   *
   * @param workingDir The path to the working directory of the repository.
   * @param arguments The list of arguments for the Git command.
   * @param output Optional pointer to a QString to receive the output message.
   * @return true if the Git command was successfully executed, false otherwise.
   */
  bool runGit(const QString& workingDir, const QStringList& arguments,
              QString* output = nullptr) const;
};