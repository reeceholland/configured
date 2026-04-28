#include "app/StatusBarController.hpp"

#include <QFileInfo>
#include <QLabel>
#include <QStatusBar>
#include <QWidget>

#include "core/ConfiguredProject.hpp"
#include "core/git/GitService.hpp"

StatusBarController::StatusBarController(GitService* gitService, QObject* parent)
    : QObject(parent), gitService_(gitService) {}

void StatusBarController::setContext(const Context& context) {
  context_ = context;
}

void StatusBarController::setWidgets(const Widgets& widgets) {
  widgets_ = widgets;
}

void StatusBarController::refresh() {
  const bool showStatus = shouldShowStatus();

  if (widgets_.statusBar) {
    widgets_.statusBar->setVisible(showStatus);
  }

  if (widgets_.refreshButton) {
    widgets_.refreshButton->setVisible(showStatus);
  }

  if (!showStatus) {
    clear();
    return;
  }

  updateRemoteUrl();
  updateBranch();
  updateDirtyState();
  updateCommitState();
}

void StatusBarController::clear() {
  if (widgets_.remoteUrlLabel) {
    widgets_.remoteUrlLabel->clear();
  }
  if (widgets_.branchLabel) {
    widgets_.branchLabel->clear();
  }
  if (widgets_.dirtyLabel) {
    widgets_.dirtyLabel->clear();
  }
  if (widgets_.commitLabel) {
    widgets_.commitLabel->clear();
  }
}

QString StatusBarController::workingDir() const {
  if (!context_.project) {
    return {};
  }

  const QString filePath = context_.projectFilePath.trimmed();
  if (filePath.isEmpty()) {
    return {};
  }
  return QFileInfo(filePath).absolutePath();
}

bool StatusBarController::shouldShowStatus() const {
  return context_.editorVisible && context_.project && context_.project->isGitManaged();
}

bool StatusBarController::isRepository(QString* output) const {
  if (!gitService_) {
    if (output) {
      *output = "Git service is not available.";
    }
    return false;
  }

  const QString dir = workingDir();
  if (dir.isEmpty()) {
    return false;
  }

  return gitService_->isRepository(dir, output);
}

void StatusBarController::updateRemoteUrl() {
  if (!widgets_.remoteUrlLabel) {
    return;
  }

  QString output;
  if (!isRepository(&output)) {
    widgets_.remoteUrlLabel->setText("Remote: Not Connected");
    return;
  }

  QString remoteUrl;
  if (gitService_->remoteUrl(workingDir(), "origin", &remoteUrl, &output)) {
    widgets_.remoteUrlLabel->setText("Remote: " + remoteUrl);
  } else {
    widgets_.remoteUrlLabel->setText("Remote: Not Connected");
  }
}

void StatusBarController::updateBranch() {
  if (!widgets_.branchLabel) {
    return;
  }

  QString output;
  if (!isRepository(&output)) {
    widgets_.branchLabel->clear();
    return;
  }

  QString branchName;
  if (gitService_->currentBranch(workingDir(), &branchName, &output) && !branchName.isEmpty()) {
    widgets_.branchLabel->setText("Branch: " + branchName);
  } else {
    widgets_.branchLabel->clear();
  }
}

void StatusBarController::updateDirtyState() {
  if (!widgets_.dirtyLabel) {
    return;
  }

  QString output;
  if (!isRepository(&output)) {
    widgets_.dirtyLabel->clear();
    return;
  }

  bool clean = false;
  if (!gitService_->workingTreeClean(workingDir(), &clean, &output)) {
    widgets_.dirtyLabel->clear();
    return;
  }

  if (clean) {
    widgets_.dirtyLabel->setText("Project: Clean");
    widgets_.dirtyLabel->setStyleSheet("color: green;");
  } else {
    widgets_.dirtyLabel->setText("Project: Dirty");
    widgets_.dirtyLabel->setStyleSheet("color: orange;");
  }
}

void StatusBarController::updateCommitState() {
  if (!widgets_.commitLabel) {
    return;
  }

  QString output;
  if (!isRepository(&output)) {
    widgets_.commitLabel->clear();
    return;
  }

  bool hasUpstream = false;
  if (!gitService_->hasUpstream(workingDir(), &hasUpstream, &output)) {
    widgets_.commitLabel->clear();
    return;
  }

  if (!hasUpstream) {
    widgets_.commitLabel->setText("First Push Needed");
    widgets_.commitLabel->setStyleSheet("color: orange;");
    return;
  }

  bool hasUnpushedCommits = false;
  if (!gitService_->hasUnpushedCommits(workingDir(), &hasUnpushedCommits, &output)) {
    widgets_.commitLabel->clear();
    return;
  }

  if (hasUnpushedCommits) {
    widgets_.commitLabel->setText("Unpushed Commits");
    widgets_.commitLabel->setStyleSheet("color: orange;");
  } else {
    widgets_.commitLabel->setText("All Commits Pushed");
    widgets_.commitLabel->setStyleSheet("color: green;");
  }
}