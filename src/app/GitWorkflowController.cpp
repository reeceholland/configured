#include "app/GitWorkflowController.hpp"

#include <QAbstractButton>
#include <QDir>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>

#include "core/ConfiguredProject.hpp"
#include "core/ProjectService.hpp"
#include "core/git/GitPullWorker.hpp"
#include "core/git/GitPushWorker.hpp"
#include "core/git/GitService.hpp"
#include "ui/NewProjectGitWizard.hpp"
#include "ui/PullPreflightDialog.hpp"
#include "ui/PullingDialog.hpp"
#include "ui/PushingDialog.hpp"

namespace {
QString findConfiguredFileInFolder(const QString& folderPath) {
  QDir dir(folderPath);
  const QStringList files = dir.entryList({"*.configured"}, QDir::Files);

  if (files.size() == 1) {
    return dir.filePath(files.first());
  }

  return {};
}
}  // namespace

GitWorkflowController::GitWorkflowController(GitService* gitService, ProjectService* projectService,
                                             QWidget* dialogParent, QObject* parent)
    : QObject(parent),
      gitService_(gitService),
      projectService_(projectService),
      dialogParent_(dialogParent) {}

void GitWorkflowController::setDialogParent(QWidget* dialogParent) {
  dialogParent_ = dialogParent;
}

void GitWorkflowController::setProjectContext(const ProjectContext& context) {
  context_ = context;
}

QString GitWorkflowController::workingDir() const {
  if (context_.projectFilePath.trimmed().isEmpty()) {
    return {};
  }

  return QFileInfo(context_.projectFilePath).absolutePath();
}

bool GitWorkflowController::ensureProjectAvailable(QString* errorMessage) const {
  if (context_.project && !context_.projectFilePath.trimmed().isEmpty()) {
    return true;
  }

  if (errorMessage) {
    *errorMessage = "Open a saved Git-managed project first.";
  }

  return false;
}

bool GitWorkflowController::ensureRepository(QString* errorMessage) const {
  QString error;
  if (!ensureProjectAvailable(&error)) {
    if (errorMessage) {
      *errorMessage = error;
    }
    return false;
  }

  if (!gitService_) {
    if (errorMessage) {
      *errorMessage = "Git service is not available.";
    }
    return false;
  }

  QString output;
  if (!gitService_->isRepository(workingDir(), &output)) {
    if (errorMessage) {
      *errorMessage = output.trimmed().isEmpty() ? "This folder is not a Git repository." : output;
    }
    return false;
  }

  return true;
}

void GitWorkflowController::showGitStatus() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Git Status", error);
    return;
  }

  QString output;
  if (!gitService_->status(workingDir(), &output)) {
    emit warningRequested("Git Status Failed", output);
    return;
  }

  if (output.trimmed().isEmpty()) {
    output = "Working tree clean.";
  }

  emit informationRequested("Git Status", output);
}

void GitWorkflowController::promptAndCommit() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Git Commit", error);
    return;
  }

  if (!projectService_ || !context_.project) {
    emit warningRequested("Git Commit", "Project service or project is not available.");
    return;
  }

  if (context_.hasUnsavedChanges && *context_.hasUnsavedChanges) {
    QMessageBox messageBox(dialogParent_);
    messageBox.setIcon(QMessageBox::Question);
    messageBox.setWindowTitle("Git Commit");
    messageBox.setText("The project has unsaved editor changes.");
    messageBox.setInformativeText("How would you like to continue?");

    QPushButton* saveAndCommitButton =
        messageBox.addButton("Save and Commit", QMessageBox::AcceptRole);
    QPushButton* commitWithoutSavingButton =
        messageBox.addButton("Commit Without Saving", QMessageBox::ActionRole);
    messageBox.addButton(QMessageBox::Cancel);

    messageBox.exec();

    if (messageBox.clickedButton() == saveAndCommitButton) {
      QString output;
      if (!projectService_->saveProject(*context_.project, context_.projectFilePath, output)) {
        emit warningRequested("Git Commit", "Project could not be saved.\n\n" + output);
        return;
      }

      if (context_.hasUnsavedChanges) {
        *context_.hasUnsavedChanges = false;
      }
    } else if (messageBox.clickedButton() != commitWithoutSavingButton) {
      return;
    }
  }

  bool ok = false;
  const QString message =
      QInputDialog::getText(dialogParent_, "Git Commit", "Commit message:", QLineEdit::Normal,
                            "Update configured project", &ok);

  if (!ok || message.trimmed().isEmpty()) {
    return;
  }

  QString output;
  if (!gitService_->addAll(workingDir(), &output)) {
    emit warningRequested("Git Commit Failed", output);
    return;
  }

  if (!gitService_->commit(workingDir(), message.trimmed(), &output)) {
    emit warningRequested("Git Commit Failed", output);
    return;
  }

  QString hash;
  QString hashOutput;
  if (context_.project && gitService_->getCommitHash(workingDir(), &hash, &hashOutput)) {
    context_.project->setGitCommitHash(hash);
  }

  emit gitStateChanged();
  emit informationRequested("Git Commit", output.isEmpty() ? "Commit created." : output);
}

void GitWorkflowController::promptAndSwitchBranch() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Switch Branch", error);
    return;
  }

  QString output;
  bool clean = false;
  if (!gitService_->workingTreeClean(workingDir(), &clean, &output)) {
    emit warningRequested("Switch Branch", "Could not determine working tree state.\n\n" + output);
    return;
  }

  if ((context_.hasUnsavedChanges && *context_.hasUnsavedChanges) || !clean) {
    emit warningRequested("Switch Branch",
                          "Save or commit your changes before switching branches.");
    return;
  }

  QStringList localBranches;
  if (!gitService_->listLocalBranches(workingDir(), &localBranches, &output)
      || localBranches.isEmpty()) {
    emit warningRequested("Switch Branch", "Could not list local branches.\n\n" + output);
    return;
  }

  QStringList remoteBranches;
  gitService_->listRemoteBranches(workingDir(), &remoteBranches, &output);

  QStringList branchChoices = localBranches;
  for (const QString& remoteBranch : remoteBranches) {
    if (!branchChoices.contains(remoteBranch)) {
      branchChoices.append(remoteBranch);
    }
  }

  QString currentBranch;
  if (!gitService_->currentBranch(workingDir(), &currentBranch, &output)
      || currentBranch.isEmpty()) {
    emit warningRequested("Switch Branch", "Could not determine current branch.\n\n" + output);
    return;
  }

  bool ok = false;
  const QString selectedBranch =
      QInputDialog::getItem(dialogParent_, "Switch Branch", "Branch:", branchChoices,
                            branchChoices.indexOf(currentBranch), false, &ok);

  if (!ok || selectedBranch.trimmed().isEmpty() || selectedBranch == currentBranch) {
    return;
  }

  const bool selectedRemoteOnly =
      remoteBranches.contains(selectedBranch) && !localBranches.contains(selectedBranch);

  const bool switched =
      selectedRemoteOnly ? gitService_->switchToRemoteBranch(workingDir(), selectedBranch, &output)
                         : gitService_->switchBranch(workingDir(), selectedBranch, &output);

  if (!switched) {
    emit warningRequested("Switch Branch", "Could not switch branch.\n\n" + output);
    return;
  }

  QString reloadProjectPath = context_.projectFilePath;
  if (!QFileInfo::exists(reloadProjectPath)) {
    reloadProjectPath = findConfiguredFileInFolder(workingDir());
  }

  if (reloadProjectPath.trimmed().isEmpty()) {
    emit warningRequested("Switch Branch",
                          "Branch switched, but no .configured project file could be found.");
    emit gitStateChanged();
    return;
  }

  if (context_.hasUnsavedChanges) {
    *context_.hasUnsavedChanges = false;
  }

  emit reloadProjectRequested(reloadProjectPath);
  emit gitStateChanged();
  emit informationRequested("Switch Branch", "Switched to branch '" + selectedBranch + "'.");
}

void GitWorkflowController::promptAndPull() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Git Pull", error);
    return;
  }

  QString output;
  if (!gitService_->remoteExists(workingDir(), "origin", &output)) {
    emit warningRequested("Git Pull", "No remote repository configured for this project.");
    return;
  }

  QString branchName;
  gitService_->currentBranch(workingDir(), &branchName, &output);

  bool clean = true;
  gitService_->workingTreeClean(workingDir(), &clean, &output);

  bool hasUpstream = false;
  gitService_->hasUpstream(workingDir(), &hasUpstream, &output);

  bool hasUnpushedCommits = false;
  gitService_->hasUnpushedCommits(workingDir(), &hasUnpushedCommits, &output);

  PullPreflightDialog::State state;
  state.hasUnsavedEditorChanges = context_.hasUnsavedChanges && *context_.hasUnsavedChanges;
  state.hasUncommittedGitChanges = !clean;
  state.hasUnpushedCommits = hasUnpushedCommits;
  state.branchName = branchName;
  state.upstreamName = hasUpstream ? "origin/" + branchName : QString();

  PullPreflightDialog dialog(state, dialogParent_);
  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  auto* pullingDialog = new PullingDialog(dialogParent_);
  pullingDialog->setStatusText("Pulling remote changes...");

  auto* thread = new QThread(this);
  auto* worker = new GitPullWorker(workingDir());
  worker->moveToThread(thread);

  connect(thread, &QThread::started, worker, &GitPullWorker::start);
  connect(pullingDialog, &PullingDialog::cancelRequested, worker, &GitPullWorker::cancel);
  connect(worker, &GitPullWorker::outputReceived, pullingDialog, &PullingDialog::appendOutput);

  connect(worker, &GitPullWorker::finished, this,
          [this, pullingDialog, thread, worker](bool success, const QString& outputText) {
            pullingDialog->appendOutput(outputText);

            thread->quit();
            worker->deleteLater();
            thread->deleteLater();

            if (!success) {
              pullingDialog->setStatusText("Pull failed. Review the output below.");
              return;
            }

            if (context_.hasUnsavedChanges) {
              *context_.hasUnsavedChanges = false;
            }

            emit reloadProjectRequested(context_.projectFilePath);
            emit gitStateChanged();
            emit informationRequested("Git Pull", "Pull completed successfully.");

            pullingDialog->setStatusText("Pull completed successfully.");
            pullingDialog->accept();
          });

  thread->start();
  pullingDialog->exec();
  pullingDialog->deleteLater();
}

void GitWorkflowController::promptAndPush() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Git Push", error);
    return;
  }

  QString output;
  if (!gitService_->remoteExists(workingDir(), "origin", &output)) {
    emit warningRequested("Git Push", "No remote repository configured for this project.");
    return;
  }

  QString branchName;
  if (!gitService_->currentBranch(workingDir(), &branchName, &output) || branchName.isEmpty()) {
    emit warningRequested("Git Push", "Could not determine current branch.\n\n" + output);
    return;
  }

  bool hasUpstream = false;
  if (!gitService_->hasUpstream(workingDir(), &hasUpstream, &output)) {
    emit warningRequested("Git Push", "Could not check upstream branch.\n\n" + output);
    return;
  }

  auto* pushingDialog = new PushingDialog(dialogParent_);
  pushingDialog->setStatusText("Pushing changes to remote...");

  auto* thread = new QThread(this);
  auto* worker = new GitPushWorker(workingDir(), !hasUpstream, branchName);
  worker->moveToThread(thread);

  connect(thread, &QThread::started, worker, &GitPushWorker::start);
  connect(pushingDialog, &PushingDialog::cancelRequested, worker, &GitPushWorker::cancel);
  connect(worker, &GitPushWorker::outputReceived, pushingDialog, &PushingDialog::appendOutput);

  connect(worker, &GitPushWorker::finished, this,
          [this, pushingDialog, thread, worker](bool success, const QString& outputText) {
            pushingDialog->appendOutput(outputText);

            thread->quit();
            worker->deleteLater();
            thread->deleteLater();

            if (!success) {
              pushingDialog->setStatusText("Push failed. Review the output below.");
              return;
            }

            emit gitStateChanged();
            emit informationRequested("Git Push", "Push completed successfully.");

            pushingDialog->setStatusText("Push completed successfully.");
            pushingDialog->accept();
          });

  thread->start();
  pushingDialog->exec();
  pushingDialog->deleteLater();
}

void GitWorkflowController::promptAndConnectRemote() {
  QString error;
  if (!ensureRepository(&error)) {
    emit warningRequested("Connect Remote", error);
    return;
  }

  bool ok = false;
  const QString remoteUrl = QInputDialog::getText(dialogParent_, "Connect Git Remote",
                                                  "Remote URL:", QLineEdit::Normal, QString(), &ok);

  if (!ok || remoteUrl.trimmed().isEmpty()) {
    return;
  }

  QString output;
  if (!gitService_->connectRemote(workingDir(), "origin", remoteUrl.trimmed(), &output)) {
    emit warningRequested("Connect Remote", output);
    return;
  }

  emit gitStateChanged();
  emit informationRequested("Connect Remote", "Remote repository connected.");
}

void GitWorkflowController::runNewProjectGitOnboarding() {
  QString error;
  if (!ensureProjectAvailable(&error)) {
    emit warningRequested("Git Setup", error);
    return;
  }

  const QString repoWorkingDir = workingDir();
  if (repoWorkingDir.isEmpty()) {
    emit warningRequested("Git Setup", "Could not determine project working directory.");
    return;
  }

  NewProjectGitWizard wizard(repoWorkingDir, gitService_, dialogParent_);
  if (wizard.exec() != QDialog::Accepted) {
    return;
  }

  const NewProjectGitWizard::State state = wizard.state();

  QString output;
  if (!state.userName.trimmed().isEmpty()
      && !gitService_->setConfigValue(repoWorkingDir, "user.name", state.userName,
                                      state.useGlobalIdentity, &output)) {
    emit warningRequested("Git Setup", "Failed to set Git user name.\n\n" + output);
  }

  if (!state.userEmail.trimmed().isEmpty()
      && !gitService_->setConfigValue(repoWorkingDir, "user.email", state.userEmail,
                                      state.useGlobalIdentity, &output)) {
    emit warningRequested("Git Setup", "Failed to set Git user email.\n\n" + output);
  }

  if (state.createInitialCommit) {
    if (!projectService_ || !context_.project
        || !projectService_->saveProject(*context_.project, context_.projectFilePath, output)) {
      emit warningRequested("Git Setup",
                            "Failed to save project before initial commit.\n\n" + output);
      return;
    }

    if (!gitService_->addAll(repoWorkingDir, &output)) {
      emit warningRequested("Git Setup", "Failed to stage files for initial commit.\n\n" + output);
      return;
    }

    if (!gitService_->commit(repoWorkingDir, state.commitMessage.trimmed(), &output)) {
      emit warningRequested("Git Setup", "Failed to create initial commit.\n\n" + output);
      return;
    }

    QString hash;
    QString hashOutput;
    if (context_.project && gitService_->getCommitHash(repoWorkingDir, &hash, &hashOutput)) {
      context_.project->setGitCommitHash(hash);
    }
  }

  if (state.connectRemote
      && !gitService_->connectRemote(repoWorkingDir, state.remoteName.trimmed(),
                                     state.remoteUrl.trimmed(), &output)) {
    emit warningRequested("Git Setup", "Could not connect remote repository.\n\n" + output);
    return;
  }

  emit gitStateChanged();

  if (state.pushAfterSetup) {
    promptAndPush();
  }
}
