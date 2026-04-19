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
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>

#include "core/ConfiguredProject.hpp"
#include "export/JsonProjectExporter.hpp"
#include "export/XmlProjectExporter.hpp"
#include "ui/EditorScreenWidget.hpp"
#include "ui/GitConfigDialog.hpp"
#include "ui/HelpScreenWidget.hpp"
#include "ui/HomeScreenWidget.hpp"
#include "ui/ProjectMetadataDialog.hpp"

MainWindow::MainWindow() {
  setWindowTitle("CONFIGURED");
  resize(800, 400);

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

  // gitInitAction_ = new QAction("Git Init", this);
  gitStatusAction_ = new QAction("Git Status", this);
  gitCommitAction_ = new QAction("Git Commit", this);
  gitConfigAction_ = new QAction("Git Identity", this);

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
  // gitMenu->addAction(gitInitAction_);
  gitMenu->addAction(gitStatusAction_);
  gitMenu->addAction(gitCommitAction_);
  gitMenu->addAction(gitConfigAction_);

  gitButton_ = new QToolButton(this);
  gitButton_->setText("Git");
  gitButton_->setMenu(gitMenu);
  gitButton_->setPopupMode(QToolButton::InstantPopup);
  gitButtonAction_ = toolbar_->addWidget(gitButton_);

  auto* aboutMenu = new QMenu(this);
  aboutMenu->addAction(versionAction_);
  aboutMenu->addAction(helpAction_);

  auto* aboutButton = new QToolButton(this);
  aboutButton->setText("About");
  aboutButton->setMenu(aboutMenu);
  aboutButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(aboutButton);

  auto* exportMenu = new QMenu(this);
  exportMenu->addAction(exportXmlAction_);
  exportMenu->addAction(exportJsonAction_);

  auto* exportButton = new QToolButton(this);
  exportButton->setText("Export");
  exportButton->setMenu(exportMenu);
  exportButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(exportButton);

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
    QDir dir(currentProjectWorkingDirectory());

    const QString filePath = dir.filePath(editor_->project()->name().trimmed() + ".configured");

    if (filePath.isEmpty()) {
      return;
    }

    if (editor_->saveProject(filePath)) {
      QMessageBox::information(this, "Save Successful", "Project saved successfully.");
    } else {
      QMessageBox::warning(this, "Save Failed", "Could not save project file.");
      return;
    }

    updateWindowTitle();
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

  connect(home_, &HomeScreenWidget::openProjectRequested, this, [this]() {
    const QString filePath = QFileDialog::getOpenFileName(
        this, "Open Configured Project", QString(),
        "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)");

    if (filePath.isEmpty()) {
      return;
    }

    if (!editor_->loadProject(filePath)) {
      QMessageBox::warning(this, "Open Failed", "Could not load project file.");
      return;
    }

    updateWindowTitle();
    updateGitUiVisibility();
    showEditor();
  });

  connect(home_, &HomeScreenWidget::createNewProjectRequested, this, [this]() {
    promptAndCreateProject();
  });

  connect(gitStatusAction_, &QAction::triggered, this, &MainWindow::onGitStatus);

  connect(gitCommitAction_, &QAction::triggered, this, &MainWindow::onGitCommit);

  connect(exportXmlAction_, &QAction::triggered, this, &MainWindow::exportParametersToXml);

  connect(exportJsonAction_, &QAction::triggered, this, &MainWindow::exportParametersToJson);

  connect(home_, &HomeScreenWidget::helpRequested, this, [this]() {
    showHelp();
  });

  showHome();
  setEditorActionsEnabled(false);
}

void MainWindow::showHome() {
  stack_->setCurrentWidget(home_);

  if (toolbar_) {
    toolbar_->setVisible(false);
  }

  if (gitButtonAction_) {
    gitButtonAction_->setVisible(false);
  }
  setEditorActionsEnabled(false);
}

void MainWindow::showEditor() {
  stack_->setCurrentWidget(editor_);

  if (toolbar_) {
    toolbar_->setVisible(true);
  }

  setEditorActionsEnabled(true);
  updateGitUiVisibility();
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
}

void MainWindow::updateWindowTitle() {
  if (editor_ && editor_->project()) {
    const QString name = editor_->project()->name().trimmed();
    if (!name.isEmpty()) {
      setWindowTitle("CONFIGURED - " + name);
      return;
    }
  }

  setWindowTitle("CONFIGURED");
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

  editor_->setProject(std::move(result.project), result.projectFilePath);

  updateWindowTitle();
  updateGitUiVisibility();
  showEditor();
}

void MainWindow::updateGitUiVisibility() {
  if (!gitButton_) {
    return;
  }

  bool showGit = false;

  if (editor_ && editor_->project()) {
    showGit = editor_->project()->isGitManaged();

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
  if (!editor_ || !editor_->project()) {
    QMessageBox::information(this, "Export", "No project is currently open.");
    return;
  }

  const QString defaultName = editor_->project()->name().trimmed().isEmpty()
                                  ? "parameters.json"
                                  : editor_->project()->name().trimmed() + "_parameters.json";

  const QString filePath = QFileDialog::getSaveFileName(this, "Export Parameters to JSON",
                                                        defaultName, "JSON Files (*.json)");

  if (filePath.isEmpty()) {
    return;
  }
  refreshProjectGitMetadata();

  JsonProjectExporter exporter;
  QString error;

  if (!exporter.exportParameters(*editor_->project(), filePath, &error)) {
    QMessageBox::warning(this, "Export Failed", error);
    return;
  }

  QMessageBox::information(this, "Export", "Parameters exported successfully.");
}

void MainWindow::onGitStatus() {
  const QString workingDir = currentProjectWorkingDirectory();

  if (workingDir.isEmpty()) {
    QMessageBox::information(this, "Git", "Save the project first before checking status.");
    return;
  }

  QString output;

  if (!gitService_.status(workingDir, &output)) {
    QMessageBox::warning(this, "Git Status Failed", output);
    return;
  }

  if (output.trimmed().isEmpty()) {
    output = "Working tree clean.";
  }

  QMessageBox::information(this, "Git Status", output);
}

void MainWindow::onGitCommit() {
  const QString workingDir = currentProjectWorkingDirectory();

  if (workingDir.isEmpty()) {
    QMessageBox::information(this, "Git", "Save the project first before committing.");
    return;
  }

  QString output;

  if (!gitService_.isRepository(workingDir, &output)) {
    QMessageBox::warning(this, "Git Commit", "This folder is not a Git repository.");
    return;
  }

  bool ok = false;
  const QString message = QInputDialog::getText(
      this, "Git Commit", "Commit message:", QLineEdit::Normal, "Update configured project", &ok);

  if (!ok || message.trimmed().isEmpty()) {
    return;
  }

  if (!editor_->saveProject(editor_->currentFilePath())) {
    QMessageBox::warning(this, "Git Commit", "Project could not be saved.");
    return;
  }

  if (!gitService_.addAll(workingDir, &output)) {
    QMessageBox::warning(this, "Git Commit Failed", output);
    return;
  }

  if (!gitService_.commit(workingDir, message.trimmed(), &output)) {
    QMessageBox::warning(this, "Git Commit Failed", output);
    return;
  }

  if (editor_->project()) {
    QString hash;
    QString hashOutput;
    if (gitService_.getCommitHash(workingDir, &hash, &hashOutput)) {
      editor_->project()->setGitCommitHash(hash);
      // Save again so the new hash is written into the .configured file
      editor_->saveProject(editor_->currentFilePath());
    }
  }
  QMessageBox::information(this, "Git Commit", output.isEmpty() ? "Commit created." : output);
}

void MainWindow::exportParametersToXml() {
  if (!editor_ || !editor_->project()) {
    QMessageBox::information(this, "Export", "No project loaded to export.");
    return;
  }

  const QString defaultName = editor_->project()->name().trimmed().isEmpty()
                                  ? "parameters.xml"
                                  : editor_->project()->name().trimmed() + "_parameters.xml";

  const QString filePath = QFileDialog::getSaveFileName(this, "Export Parameters to XML",
                                                        defaultName, "XML Files (*.xml)");

  if (filePath.isEmpty()) {
    return;
  }

  refreshProjectGitMetadata();

  XmlProjectExporter exporter;
  QString error;

  if (!exporter.exportParameters(*editor_->project(), filePath, &error)) {
    QMessageBox::warning(this, "Export Failed", error);
    return;
  }

  QMessageBox::information(this, "Export", "Parameters exported successfully.");
}

void MainWindow::refreshProjectGitMetadata() {
  if (!editor_ || !editor_->project()) {
    return;
  }

  auto* project = editor_->project();
  if (!project->isGitManaged()) {
    return;
  }

  const QString workingDir = currentProjectWorkingDirectory();
  if (workingDir.isEmpty()) {
    return;
  }

  QString hash;
  QString output;
  if (gitService_.getCommitHash(workingDir, &hash, &output) && !hash.isEmpty()) {
    project->setGitCommitHash(hash);
  }
}

QString MainWindow::currentProjectWorkingDirectory() const {
  if (!editor_ || !editor_->project()) {
    return {};
  }

  const QString filePath = editor_->currentFilePath();
  if (filePath.isEmpty()) {
    return {};
  }

  return QFileInfo(filePath).absolutePath();
}

void MainWindow::editProjectMetadata() {
  if (!editor_ || !editor_->project()) {
    return;
  }

  refreshProjectGitMetadata();

  // Build initial metadata
  ProjectMetadata initial;
  initial.name = editor_->project()->name();
  initial.description = editor_->project()->description();
  initial.author = editor_->project()->author();
  initial.company = editor_->project()->company();
  initial.version = editor_->project()->version();
  initial.robotPlatform = editor_->project()->robotPlatform();
  initial.gitManaged = editor_->project()->isGitManaged();
  initial.lastModified = editor_->project()->lastModified();
  initial.gitCommitHash = editor_->project()->gitCommitHash();

  ProjectMetadataDialog dialog(initial, this);

  if (dialog.exec() != QDialog::Accepted) {
    return;
  }

  const ProjectMetadata updated = dialog.metadata();

  QString error;
  if (!projectService_.updateProjectMetadata(*editor_->project(), updated,
                                             editor_->currentFilePath(), error)) {
    QMessageBox::warning(this, "Metadata Update Failed", error);
    return;
  }

  updateWindowTitle();
  updateGitUiVisibility();
}