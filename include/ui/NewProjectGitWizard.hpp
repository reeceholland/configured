#pragma once

#include <QString>
#include <QWizard>

#include "core/git/GitService.hpp"

class NewProjectGitWizard : public QWizard {
  Q_OBJECT
 public:
  struct State {
    QString workingDir;
    QString branchName;

    QString userName;
    QString userEmail;
    bool useGlobalIdentity = false;

    bool createInitialCommit = true;
    QString commitMessage = "Initial configured project";

    bool connectRemote = false;
    QString remoteName = "origin";
    QString remoteUrl;

    bool pushAfterSetup = false;
  };

  explicit NewProjectGitWizard(const QString& workingDir, GitService* service,
                               QWidget* parent = nullptr);

  State state() const;

 private:
  void loadInitialState();
  void collectStateFromFields();

  GitService* gitService_ = nullptr;
  State state_;
};