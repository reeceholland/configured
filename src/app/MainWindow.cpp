/**
 * @file MainWindow.cpp
 * @author Reece Holland
 * @brief Implementation of the main window of the Configured application.
 * @version 0.3
 * @date 2026-04-15
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#include "app/MainWindow.hpp"

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>
#include <QThread>
#include <QToolBar>
#include <QToolButton>

#include "app/GitWorkflowController.hpp"
#include "app/StatusBarController.hpp"
#include "core/ConfiguredProject.hpp"
#include "core/git/CloneWorker.hpp"
#include "export/JsonProjectExporter.hpp"
#include "export/XmlProjectExporter.hpp"
#include "ui/ConnectingDialog.hpp"
#include "ui/EditorScreenWidget.hpp"
#include "ui/GitConfigDialog.hpp"
#include "ui/HelpScreenWidget.hpp"
#include "ui/HomeScreenWidget.hpp"
#include "ui/ProjectMetadataDialog.hpp"
#include "ui/RemoteConnectDialog.hpp"

MainWindow::MainWindow() {
  setWindowTitle("CONFIGURED");
  resize(800, 400);

  gitWorkflowController_ =
      std::make_unique<GitWorkflowController>(&gitService_, &projectService_, this);

  connect(gitWorkflowController_.get(), &GitWorkflowController::informationRequested, this,
          [this](const QString& title, const QString& message) {
            QMessageBox::information(this, title, message);
          });

  connect(gitWorkflowController_.get(), &GitWorkflowController::warningRequested, this,
          [this](const QString& title, const QString& message) {
            QMessageBox::warning(this, title, message);
          });

  connect(gitWorkflowController_.get(), &GitWorkflowController::gitStateChanged, this, [this]() {
    updateWindowTitle();
    updateGitUiVisibility();
    updateGitStatusBar();
  });

  connect(gitWorkflowController_.get(), &GitWorkflowController::reloadProjectRequested, this,
          [this](const QString& projectFilePath) {
            QString error;
            auto reloadedProject = projectService_.loadProject(projectFilePath, error);
            if (!reloadedProject) {
              QMessageBox::warning(this, "Reload Project Failed", error);
              return;
            }

            currentProject_ = std::move(reloadedProject);
            currentProjectFilePath_ = projectFilePath;
            currentProject_->clearDirtyFlags();
            hasUnsavedChanges_ = false;
            editor_->setProject(currentProject_.get());
            gitWorkflowController_->setProjectContext(
                {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
            updateWindowTitle();
            updateGitUiVisibility();
            updateGitStatusBar();
          });

  this->setStyleSheet(R"(
      QToolButton::menu-indicator {
          image: none;
      }
  )");

  stack_ = new QStackedWidget(this);
  setCentralWidget(stack_);

  home_ = new HomeScreenWidget(this);
  editor_ = new EditorScreenWidget(this);
  help_ = new HelpScreenWidget(this);

  stack_->addWidget(home_);
  stack_->addWidget(editor_);
  stack_->addWidget(help_);

  toolbar_ = addToolBar("Main");
  toolbar_->setMovable(false);

  saveProjectAction_ = new QAction("Save Project", this);
  addChildAction_ = new QAction("Add Child", this);
  removeItemAction_ = new QAction("Remove Selected", this);
  goHomeAction_ = new QAction("Home", this);
  projectMetadataAction_ = new QAction("Project Metadata", this);
  versionAction_ = new QAction("Version", this);
  helpAction_ = new QAction("Help", this);
  exportXmlAction_ = new QAction("Export to XML", this);
  exportJsonAction_ = new QAction("Export to JSON", this);

  gitStatusAction_ = new QAction("Git Status", this);
  gitCommitAction_ = new QAction("Git Commit", this);
  gitConfigAction_ = new QAction("Git Identity", this);
  gitConnectRemoteAction_ = new QAction("Connect Remote", this);
  gitSwitchBranchAction_ = new QAction("Switch Branch", this);
  gitPullAction_ = new QAction("Git Pull", this);
  gitPushAction_ = new QAction("Git Push", this);

  remoteUrlStatusLabel_ = new QLabel(this);
  statusBar()->addPermanentWidget(remoteUrlStatusLabel_);

  gitBranchLabel_ = new QLabel(this);
  statusBar()->addPermanentWidget(gitBranchLabel_);

  projectDirtyStatusLabel_ = new QLabel(this);
  statusBar()->addPermanentWidget(projectDirtyStatusLabel_);

  unpushedCommitsLabel_ = new QLabel(this);
  statusBar()->addPermanentWidget(unpushedCommitsLabel_);

  gitRefreshButton_ = new QToolButton(this);
  gitRefreshButton_->setText("Refresh");
  gitRefreshButton_->setToolTip("Refresh Git status");
  gitRefreshButton_->setAutoRaise(true);
  gitRefreshButton_->setStyleSheet(R"(
    QToolButton {
      background-color: #1f1f1f;
      border: 1px solid #b8b8b8;
      border-radius: 3px;
      padding: 1px 2px;
      color: #ffffff;
    }

    QToolButton:hover {
      background-color: #5a5a5a;
    }

    QToolButton:pressed {
      background-color: #5a5a5a;
    }
  )");
  statusBar()->addPermanentWidget(gitRefreshButton_);
  gitRefreshButton_->setVisible(false);

  statusBar()->setStyleSheet(R"(
    QStatusBar::item {
      border: none;
    }
  )");

  statusBarController_ = std::make_unique<StatusBarController>(&gitService_, this);
  statusBarController_->setWidgets({statusBar(), remoteUrlStatusLabel_, gitBranchLabel_,
                                    projectDirtyStatusLabel_, unpushedCommitsLabel_,
                                    gitRefreshButton_});

  auto* projectMenu = new QMenu(this);
  projectMenu->addAction(saveProjectAction_);
  projectMenu->addAction(projectMetadataAction_);
  projectMenu->addSeparator();
  projectMenu->addAction(goHomeAction_);

  auto* projectButton = new QToolButton(this);
  projectButton->setText("Project");
  projectButton->setMenu(projectMenu);
  projectButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(projectButton);

  auto* editMenu = new QMenu(this);
  editMenu->addAction(addChildAction_);
  editMenu->addAction(removeItemAction_);

  auto* editButton = new QToolButton(this);
  editButton->setText("Edit");
  editButton->setMenu(editMenu);
  editButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(editButton);

  auto* gitMenu = new QMenu(this);
  gitMenu->addAction(gitConnectRemoteAction_);
  gitMenu->addAction(gitSwitchBranchAction_);
  gitMenu->addAction(gitPullAction_);
  gitMenu->addAction(gitPushAction_);
  gitMenu->addSeparator();
  gitMenu->addAction(gitStatusAction_);
  gitMenu->addAction(gitCommitAction_);
  gitMenu->addAction(gitConfigAction_);

  gitButton_ = new QToolButton(this);
  gitButton_->setText("Git");
  gitButton_->setMenu(gitMenu);
  gitButton_->setPopupMode(QToolButton::InstantPopup);
  gitButtonAction_ = toolbar_->addWidget(gitButton_);

  auto* exportMenu = new QMenu(this);
  exportMenu->addAction(exportXmlAction_);
  exportMenu->addAction(exportJsonAction_);

  auto* exportButton = new QToolButton(this);
  exportButton->setText("Export");
  exportButton->setMenu(exportMenu);
  exportButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(exportButton);

  auto* aboutMenu = new QMenu(this);
  aboutMenu->addAction(versionAction_);
  aboutMenu->addAction(helpAction_);

  auto* aboutButton = new QToolButton(this);
  aboutButton->setText("About");
  aboutButton->setMenu(aboutMenu);
  aboutButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(aboutButton);

  connect(helpAction_, &QAction::triggered, this, [this]() {
    showHelp();
  });

  connect(help_, &HelpScreenWidget::backRequested, this, [this]() {
    QWidget* previous = lastCentral_;
    lastCentral_ = nullptr;

    if (previous == editor_) {
      showEditor();
    } else {
      showHome();
    }
  });

  connect(versionAction_, &QAction::triggered, this, [this]() {
    QMessageBox::information(this, "About CONFIGURED",
                             QString("CONFIGURED Version %1").arg(APP_VERSION));
  });

  connect(gitConfigAction_, &QAction::triggered, this, [this]() {
    QString output;
    if (!gitService_.isGitAvailable(&output)) {
      QMessageBox::warning(this, "Git", "Git is not available.\n\n" + output);
      return;
    }

    const QString workingDir = currentProjectWorkingDirectory();

    GitConfigDialog dialog(&gitService_, workingDir, this);
    dialog.exec();
  });

  connect(saveProjectAction_, &QAction::triggered, this, [this]() {
    if (!currentProject_) {
      QMessageBox::warning(this, "Save Failed", "No project is currently open.");
      return;
    }

    // MainWindow owns the active project session; ProjectService owns persistence.
    QString error;
    if (!projectService_.saveProject(*currentProject_, currentProjectFilePath_, error)) {
      QMessageBox::warning(this, "Save Failed", error);
      return;
    }

    QMessageBox::information(this, "Save Successful", "Project saved successfully.");
    currentProject_->clearDirtyFlags();
    editor_->refreshTree();
    hasUnsavedChanges_ = false;
    updateWindowTitle();
    updateGitStatusBar();
  });

  connect(addChildAction_, &QAction::triggered, this, [this]() {
    editor_->addChildToSelected();
  });

  connect(removeItemAction_, &QAction::triggered, this, [this]() {
    editor_->removeSelectedItem();
  });

  connect(goHomeAction_, &QAction::triggered, this, [this]() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Go Home",
        "Are you sure you want to return to the home screen? Unsaved changes will be lost.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
      showHome();
    }
  });

  connect(projectMetadataAction_, &QAction::triggered, this, &MainWindow::editProjectMetadata);

  connect(gitPullAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->promptAndPull();
  });

  connect(gitPushAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->promptAndPush();
  });

  connect(home_, &HomeScreenWidget::openProjectRequested, this, [this]() {
    const QString filePath = QFileDialog::getOpenFileName(
        this, "Open Configured Project", QString(),
        "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
      return;
    }

    QString error;
    auto loadedProject = projectService_.loadProject(filePath, error);

    if (!loadedProject) {
      QMessageBox::warning(this, "Open Failed", error);
      return;
    }

    // Store the loaded project before handing the editor a non-owning pointer.
    currentProject_ = std::move(loadedProject);
    currentProjectFilePath_ = filePath;

    editor_->setProject(currentProject_.get());

    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});

    updateGitStatusBar();
    updateWindowTitle();
    updateGitUiVisibility();
    showEditor();
  });

  connect(home_, &HomeScreenWidget::createNewProjectRequested, this, [this]() {
    promptAndCreateProject();
  });

  connect(gitConnectRemoteAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->promptAndConnectRemote();
  });
  connect(gitSwitchBranchAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->promptAndSwitchBranch();
  });

  connect(gitStatusAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->showGitStatus();
  });

  connect(gitCommitAction_, &QAction::triggered, this, [this]() {
    gitWorkflowController_->setProjectContext(
        {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});
    gitWorkflowController_->promptAndCommit();
  });

  connect(exportXmlAction_, &QAction::triggered, this, &MainWindow::exportParametersToXml);

  connect(exportJsonAction_, &QAction::triggered, this, &MainWindow::exportParametersToJson);

  connect(home_, &HomeScreenWidget::helpRequested, this, [this]() {
    showHelp();
  });

  connect(home_, &HomeScreenWidget::connectRemoteRequested, this, [this]() {
    promptAndCloneRemoteProject();
  });

  connect(editor_, &EditorScreenWidget::projectModified, this, [this](ConfiguredItem* item) {
    hasUnsavedChanges_ = true;

    if (item) {
      statusBar()->showMessage("Unsaved changes", 2000);
    }

    updateWindowTitle();
  });

  connect(gitRefreshButton_, &QToolButton::clicked, this, [this]() {
    updateGitStatusBar();
  });

  showHome();
  setEditorActionsEnabled(false);
}

MainWindow::~MainWindow() = default;

void MainWindow::showHome() {
  stack_->setCurrentWidget(home_);

  if (toolbar_) {
    toolbar_->setVisible(false);
  }

  if (gitButtonAction_) {
    gitButtonAction_->setVisible(false);
  }

  setEditorActionsEnabled(false);
  updateGitStatusBar();
}

void MainWindow::showEditor() {
  stack_->setCurrentWidget(editor_);

  if (toolbar_) {
    toolbar_->setVisible(true);
  }

  setEditorActionsEnabled(true);
  updateGitUiVisibility();
  updateGitStatusBar();
}

void MainWindow::setEditorActionsEnabled(bool enabled) {
  if (saveProjectAction_) {
    saveProjectAction_->setEnabled(enabled);
  }

  if (addChildAction_) {
    addChildAction_->setEnabled(enabled);
  }

  if (removeItemAction_) {
    removeItemAction_->setEnabled(enabled);
  }

  if (goHomeAction_) {
    goHomeAction_->setEnabled(enabled);
  }

  if (projectMetadataAction_) {
    projectMetadataAction_->setEnabled(enabled);
  }

  if (gitStatusAction_) {
    gitStatusAction_->setEnabled(enabled);
  }

  if (gitCommitAction_) {
    gitCommitAction_->setEnabled(enabled);
  }

  if (gitConfigAction_) {
    gitConfigAction_->setEnabled(enabled);
  }

  if (exportXmlAction_) {
    exportXmlAction_->setEnabled(enabled);
  }

  if (exportJsonAction_) {
    exportJsonAction_->setEnabled(enabled);
  }

  if (gitConnectRemoteAction_) {
    gitConnectRemoteAction_->setEnabled(enabled);
  }

  if (gitPullAction_) {
    gitPullAction_->setEnabled(enabled);
  }

  if (gitSwitchBranchAction_) {
    gitSwitchBranchAction_->setEnabled(enabled);
  }

  if (gitRefreshButton_) {
    gitRefreshButton_->setEnabled(enabled);
  }
}

void MainWindow::updateWindowTitle() {
  QString title = "CONFIGURED";

  if (currentProject_) {
    const QString name = currentProject_->name().trimmed();
    if (!name.isEmpty()) {
      title += " - " + name;
    }
  }

  if (hasUnsavedChanges_) {
    title += " *";
  }

  setWindowTitle(title);
}

void MainWindow::promptAndCreateProject() {
  ProjectMetadata initial;
  initial.name = "New Project";
  initial.gitManaged = false;

  ProjectMetadataDialog dialog(initial, this);
  if (dialog.exec() != QDialog::Accepted) {
    showHome();
    return;
  }

  const ProjectMetadata metadata = dialog.metadata();

  if (metadata.name.trimmed().isEmpty()) {
    QMessageBox::warning(this, "New Project", "Project name cannot be empty.");
    showHome();
    return;
  }

  const QString baseFolder =
      QFileDialog::getExistingDirectory(this, "Select Parent Folder for Project");

  if (baseFolder.isEmpty()) {
    showHome();
    return;
  }

  ProjectCreationResult result = projectService_.createProject(metadata, baseFolder);

  if (!result.success) {
    QMessageBox::warning(this, "Project Creation Failed", result.errorMessage);
    showHome();
    return;
  }

  // ProjectService creates the project and file; MainWindow owns the active session.
  currentProject_ = std::move(result.project);
  currentProjectFilePath_ = result.projectFilePath;

  editor_->setProject(currentProject_.get());

  gitWorkflowController_->setProjectContext(
      {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});

  updateGitStatusBar();
  updateWindowTitle();
  updateGitUiVisibility();
  showEditor();

  if (!currentProject_ || !currentProject_->isGitManaged()) {
    return;
  }
  gitWorkflowController_->runNewProjectGitOnboarding();
}
void MainWindow::updateGitUiVisibility() {
  if (!gitButton_) {
    return;
  }

  bool showGit = false;

  if (currentProject_) {
    showGit = currentProject_->isGitManaged();

    if (showGit) {
      const QString workingDir = currentProjectWorkingDirectory();
      QString output;
      if (workingDir.isEmpty() || !gitService_.isGitAvailable(&output)
          || !gitService_.isRepository(workingDir, &output)) {
        showGit = false;
      }
    }
  }

  if (gitButtonAction_) {
    gitButtonAction_->setVisible(showGit);
  }
}

void MainWindow::showHelp() {
  lastCentral_ = stack_->currentWidget();

  stack_->setCurrentWidget(help_);
  if (toolbar_) {
    toolbar_->setVisible(false);
  }
}

void MainWindow::exportParametersToJson() {
  if (!currentProject_) {
    QMessageBox::information(this, "Export", "No project is currently open.");
    return;
  }

  const QString defaultName = currentProject_->name().trimmed().isEmpty()
                                  ? "parameters.json"
                                  : currentProject_->name().trimmed() + "_parameters.json";

  const QString filePath = QFileDialog::getSaveFileName(this, "Export Parameters to JSON",
                                                        defaultName, "JSON Files (*.json)");

  if (filePath.isEmpty()) {
    return;
  }
  refreshProjectGitMetadata();

  JsonProjectExporter exporter;
  QString error;

  if (!exporter.exportParameters(*currentProject_, filePath, &error)) {
    QMessageBox::warning(this, "Export Failed", error);
    return;
  }

  QMessageBox::information(this, "Export", "Parameters exported successfully.");
}

void MainWindow::exportParametersToXml() {
  if (!currentProject_) {
    QMessageBox::information(this, "Export", "No project loaded to export.");
    return;
  }

  const QString defaultName = currentProject_->name().trimmed().isEmpty()
                                  ? "parameters.xml"
                                  : currentProject_->name().trimmed() + "_parameters.xml";

  const QString filePath = QFileDialog::getSaveFileName(this, "Export Parameters to XML",
                                                        defaultName, "XML Files (*.xml)");

  if (filePath.isEmpty()) {
    return;
  }

  refreshProjectGitMetadata();

  XmlProjectExporter exporter;
  QString error;

  if (!exporter.exportParameters(*currentProject_, filePath, &error)) {
    QMessageBox::warning(this, "Export Failed", error);
    return;
  }

  QMessageBox::information(this, "Export", "Parameters exported successfully.");
}

void MainWindow::refreshProjectGitMetadata() {
  if (!currentProject_) {
    return;
  }

  if (!currentProject_->isGitManaged()) {
    return;
  }

  const QString workingDir = currentProjectWorkingDirectory();
  if (workingDir.isEmpty()) {
    return;
  }

  QString hash;
  QString output;
  if (gitService_.getCommitHash(workingDir, &hash, &output) && !hash.isEmpty()) {
    currentProject_->setGitCommitHash(hash);
  }
}

QString MainWindow::currentProjectWorkingDirectory() const {
  if (!currentProject_) {
    return {};
  }

  // The folder containing the .configured file is the Git working directory.
  const QString filePath = currentProjectFilePath_.trimmed();
  if (filePath.isEmpty()) {
    return {};
  }

  return QFileInfo(filePath).absolutePath();
}

void MainWindow::editProjectMetadata() {
  if (!currentProject_) {
    return;
  }

  refreshProjectGitMetadata();

  // Build initial metadata
  ProjectMetadata initial;
  initial.name = currentProject_->name();
  initial.description = currentProject_->description();
  initial.author = currentProject_->author();
  initial.company = currentProject_->company();
  initial.version = currentProject_->version();
  initial.robotPlatform = currentProject_->robotPlatform();
  initial.gitManaged = currentProject_->isGitManaged();
  initial.lastModified = currentProject_->lastModified();
  initial.gitCommitHash = currentProject_->gitCommitHash();

  ProjectMetadataDialog dialog(initial, this);

  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  const ProjectMetadata updated = dialog.metadata();

  QString error;
  if (!projectService_.updateProjectMetadata(*currentProject_, updated,
                                             currentProjectFilePath_.trimmed(), error)) {
    QMessageBox::warning(this, "Metadata Update Failed", error);
    return;
  }

  if (!projectService_.saveProject(*currentProject_, currentProjectFilePath_, error)) {
    QMessageBox::warning(this, "Metadata Update Failed", error);
    return;
  }

  updateWindowTitle();
  updateGitUiVisibility();
  updateGitStatusBar();
}

void MainWindow::promptAndCloneRemoteProject() {
  RemoteConnectDialog dialog(this);

  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  QString output;
  if (!gitService_.isGitAvailable(&output)) {
    QMessageBox::warning(this, "Connect Remote", "Git is not available.\n\n" + output);
    return;
  }

  auto* connectingDialog = new ConnectingDialog(this);
  auto* thread = new QThread(this);
  auto* worker = new CloneWorker(dialog.remoteUrl(), dialog.parentFolder());

  worker->moveToThread(thread);

  connect(thread, &QThread::started, worker, &CloneWorker::start);

  connect(connectingDialog, &ConnectingDialog::cancelRequested, worker, &CloneWorker::cancel);

  connect(worker, &CloneWorker::outputReceived, connectingDialog, &ConnectingDialog::appendOutput);

  connect(worker, &CloneWorker::finished, this,
          [this, connectingDialog, thread, worker](bool success, const QString& clonedPath,
                                                   const QString& output) {
            connectingDialog->appendOutput(output);
            connectingDialog->accept();

            thread->quit();
            worker->deleteLater();
            thread->deleteLater();
            connectingDialog->deleteLater();

            if (!success) {
              QMessageBox::warning(this, "Connect Remote",
                                   "Repository could not be cloned.\n\n" + output);
              return;
            }

            const QString projectFilePath = findConfiguredFile(clonedPath);
            if (projectFilePath.isEmpty()) {
              QMessageBox::warning(this, "Connect Remote",
                                   "Repository cloned, but no .configured file was found.");
              return;
            }

            QString error;
            auto loadedProject = projectService_.loadProject(projectFilePath, error);

            if (!loadedProject) {
              QMessageBox::warning(this, "Open Failed", error);
              return;
            }

            currentProject_ = std::move(loadedProject);
            currentProjectFilePath_ = projectFilePath;

            editor_->setProject(currentProject_.get());
            gitWorkflowController_->setProjectContext(
                {currentProject_.get(), currentProjectFilePath_, &hasUnsavedChanges_});

            updateWindowTitle();
            updateGitUiVisibility();
            showEditor();
          });

  connect(worker, &CloneWorker::finished, thread, &QThread::quit);

  thread->start();
  connectingDialog->exec();
}

QString MainWindow::findConfiguredFile(const QString& folderPath) const {
  QDir dir(folderPath);
  const QStringList files = dir.entryList({"*.configured"}, QDir::Files);

  if (files.size() == 1) {
    return dir.filePath(files.first());
  }

  return {};
}

void MainWindow::updateGitStatusBar() {
  if (!statusBarController_) {
    return;
  }

  statusBarController_->setContext({currentProject_.get(), currentProjectFilePath_,
                                    hasUnsavedChanges_,
                                    stack_ && stack_->currentWidget() == editor_});

  statusBarController_->refresh();
}