#pragma once

#include <QString>
#include <QWizard>

#include "core/git/GitService.hpp"

/**
 * @brief Wizard for configuring Git immediately after creating a project.
 *
 * The wizard captures identity, initial commit, remote, and push preferences.
 * GitWorkflowController reads the final State and performs the requested work.
 */
class NewProjectGitWizard : public QWizard {
  Q_OBJECT
 public:
  /**
   * @brief Collected onboarding choices.
   */
  struct State {
    /// Repository/project working directory.
    QString workingDir;

    /// Current branch name shown in the wizard.
    QString branchName;

    /// Git user.name value to apply locally or globally.
    QString userName;

    /// Git user.email value to apply locally or globally.
    QString userEmail;

    /// True when identity should be written to the user's global Git config.
    bool useGlobalIdentity = false;

    /// True when the wizard should create the first commit.
    bool createInitialCommit = true;

    /// Commit message used for the initial commit.
    QString commitMessage = "Initial configured project";

    /// True when a remote should be configured.
    bool connectRemote = false;

    /// Remote name, usually origin.
    QString remoteName = "origin";

    /// Remote repository URL.
    QString remoteUrl;

    /// True when the current branch should be pushed after setup.
    bool pushAfterSetup = false;
  };

  /**
   * @brief Create the onboarding wizard.
   * @param workingDir Project repository directory.
   * @param service Git service used to prefill existing identity/branch state.
   * @param parent Parent widget.
   */
  explicit NewProjectGitWizard(const QString& workingDir, GitService* service,
                               QWidget* parent = nullptr);

  /// Return the final wizard state after the dialog is accepted.
  State state() const;

 private:
  void loadInitialState();
  void collectStateFromFields();

  GitService* gitService_ = nullptr;
  State state_;
};
