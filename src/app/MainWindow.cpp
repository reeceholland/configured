#include "app/MainWindow.hpp"

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>
#include <QDir>
#include <QDebug>

#include "core/ConfiguredProject.hpp"
#include "ui/EditorScreenWidget.hpp"
#include "ui/GitConfigDialog.hpp"
#include "ui/HomeScreenWidget.hpp"
#include "ui/HelpScreenWidget.hpp"
#include "ui/ProjectMetadataDialog.hpp"
#include "export/XmlProjectExporter.hpp"

MainWindow::MainWindow()
{
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

  gitInitAction_ = new QAction("Git Init", this);
  gitStatusAction_ = new QAction("Git Status", this);
  gitCommitAction_ = new QAction("Git Commit", this);
  gitConfigAction_ = new QAction("Git Identity", this);

  auto *projectMenu = new QMenu(this);
  projectMenu->addAction(saveProjectAction_);
  projectMenu->addAction(projectMetadataAction_);
  projectMenu->addSeparator();
  projectMenu->addAction(goHomeAction_);

  auto *projectButton = new QToolButton(this);
  projectButton->setText("Project");
  projectButton->setMenu(projectMenu);
  projectButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(projectButton);

  auto *editMenu = new QMenu(this);
  editMenu->addAction(addChildAction_);
  editMenu->addAction(removeItemAction_);

  auto *editButton = new QToolButton(this);
  editButton->setText("Edit");
  editButton->setMenu(editMenu);
  editButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(editButton);

  auto *gitMenu = new QMenu(this);
  gitMenu->addAction(gitInitAction_);
  gitMenu->addAction(gitStatusAction_);
  gitMenu->addAction(gitCommitAction_);
  gitMenu->addAction(gitConfigAction_);

  gitButton_ = new QToolButton(this);
  gitButton_->setText("Git");
  gitButton_->setMenu(gitMenu);
  gitButton_->setPopupMode(QToolButton::InstantPopup);
  gitButtonAction_ = toolbar_->addWidget(gitButton_);

  auto *aboutMenu = new QMenu(this);
  aboutMenu->addAction(versionAction_);
  aboutMenu->addAction(helpAction_);

  auto *aboutButton = new QToolButton(this);
  aboutButton->setText("About");
  aboutButton->setMenu(aboutMenu);
  aboutButton->setPopupMode(QToolButton::InstantPopup);
  toolbar_->addWidget(aboutButton);

  connect(helpAction_, &QAction::triggered, this, [this]()
          { showHelp(); });

  connect(help_, &HelpScreenWidget::backRequested, this, [this]()
          { showEditor(); });

  connect(versionAction_, &QAction::triggered, this, [this]()
          { QMessageBox::information(this, "About CONFIGURED", QString("CONFIGURED Version %1").arg(APP_VERSION)); });

  connect(gitConfigAction_, &QAction::triggered, this, [this]()
          {
      QString output;
      if (!gitService_.isGitAvailable(&output))
      {
        QMessageBox::warning(this, "Git", "Git is not available.\n\n" + output);
        return;
      }

      const QString workingDir = currentProjectWorkingDirectory();

      GitConfigDialog dialog(&gitService_, workingDir, this);
      dialog.exec(); });

  connect(saveProjectAction_, &QAction::triggered, this, [this]()
          {
      const QString filePath = QFileDialog::getSaveFileName(
          this,
          "Save Configured Project",
          QString(),
          "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)");

      if (filePath.isEmpty())
      {
        return;
      }

      if (!editor_->saveProject(filePath))
      {
        QMessageBox::warning(this, "Save Failed", "Could not save project file.");
        return;
      }

      updateWindowTitle(); });

  connect(addChildAction_, &QAction::triggered, this, [this]()
          { editor_->addChildToSelected(); });

  connect(removeItemAction_, &QAction::triggered, this, [this]()
          { editor_->removeSelectedItem(); });

  connect(goHomeAction_, &QAction::triggered, this, [this]()
          {
      QMessageBox::StandardButton reply = QMessageBox::question(
          this,
          "Go Home",
          "Are you sure you want to return to the home screen? Unsaved changes will be lost.",
          QMessageBox::Yes | QMessageBox::No);

      if (reply == QMessageBox::Yes)
      {
        showHome();
      } });

  connect(projectMetadataAction_, &QAction::triggered, this, [this]()
          {
      if (!editor_ || !editor_->project())
      {
        return;
      }

      ProjectMetadataDialog dialog(
          editor_->project(),
          currentProjectWorkingDirectory(),
          &gitService_,
          this);

      if (dialog.exec() == QDialog::Accepted)
      {
        updateWindowTitle();
        updateGitUiVisibility();
      } });

  connect(home_, &HomeScreenWidget::openProjectRequested, this, [this]()
          {
      const QString filePath = QFileDialog::getOpenFileName(
          this,
          "Open Configured Project",
          QString(),
          "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)");

      if (filePath.isEmpty())
      {
        return;
      }

      if (!editor_->loadProject(filePath))
      {
        QMessageBox::warning(this, "Open Failed", "Could not load project file.");
        return;
      }

      updateWindowTitle();
      updateGitUiVisibility();
      showEditor(); });

  connect(home_, &HomeScreenWidget::createNewProjectRequested, this, [this]()
          { promptAndCreateProject(); });

  connect(gitInitAction_, &QAction::triggered, this, [this]()
          {
      const QString workingDir = currentProjectWorkingDirectory();

      if (workingDir.isEmpty())
      {
        QMessageBox::information(this, "Git", "Save the project first before initializing Git.");
        return;
      }

      QString output;

      if (!gitService_.isGitAvailable(&output))
      {
        QMessageBox::warning(this, "Git", "Git is not available.\n\n" + output);
        return;
      }

      if (gitService_.isRepository(workingDir, &output))
      {
        QMessageBox::information(this, "Git", "This folder is already a Git repository.");

        if (editor_->project())
        {
          editor_->project()->setGitManaged(true);
        }

        return;
      }

      if (!gitService_.initRepository(workingDir, &output))
      {
        QMessageBox::warning(this, "Git Init Failed", output);
        return;
      }

      if (editor_->project())
      {
        editor_->project()->setGitManaged(true);
      }

      QMessageBox::information(
          this,
          "Git Init",
          output.isEmpty() ? "Repository initialized." : output); });

  connect(gitStatusAction_, &QAction::triggered, this, [this]()
          {
      const QString workingDir = currentProjectWorkingDirectory();

      if (workingDir.isEmpty())
      {
        QMessageBox::information(this, "Git", "Save the project first before checking status.");
        return;
      }

      QString output;

      if (!gitService_.status(workingDir, &output))
      {
        QMessageBox::warning(this, "Git Status Failed", output);
        return;
      }

      if (output.trimmed().isEmpty())
      {
        output = "Working tree clean.";
      }

      QMessageBox::information(this, "Git Status", output); });

  connect(gitCommitAction_, &QAction::triggered, this, [this]()
          {
      const QString workingDir = currentProjectWorkingDirectory();

      if (workingDir.isEmpty())
      {
        QMessageBox::information(this, "Git", "Save the project first before committing.");
        return;
      }

      QString output;

      if (!gitService_.isRepository(workingDir, &output))
      {
        QMessageBox::warning(this, "Git Commit", "This folder is not a Git repository.");
        return;
      }

      bool ok = false;
      const QString message = QInputDialog::getText(
          this,
          "Git Commit",
          "Commit message:",
          QLineEdit::Normal,
          "Update configured project",
          &ok);

      if (!ok || message.trimmed().isEmpty())
      {
        return;
      }

      if (!editor_->saveProject(editor_->currentFilePath()))
      {
        QMessageBox::warning(this, "Git Commit", "Project could not be saved.");
        return;
      }

      if (!gitService_.addAll(workingDir, &output))
      {
        QMessageBox::warning(this, "Git Commit Failed", output);
        return;
      }

      if (!gitService_.commit(workingDir, message.trimmed(), &output))
      {
        QMessageBox::warning(this, "Git Commit Failed", output);
        return;
      }

      if (editor_->project())
      {
        QString hash;
        QString hashOutput;
        if (gitService_.getCommitHash(workingDir, &hash, &hashOutput))
        {
          editor_->project()->setGitCommitHash(hash);

          // Save again so the new hash is written into the .configured file
          editor_->saveProject(editor_->currentFilePath());
        }
      }
      QMessageBox::information(
          this,
          "Git Commit",
          output.isEmpty() ? "Commit created." : output); });

  showHome();
  setEditorActionsEnabled(false);
}

void MainWindow::showHome()
{
  stack_->setCurrentWidget(home_);

  if (toolbar_)
  {
    toolbar_->setVisible(false);
  }

  if (gitButtonAction_)
  {
    gitButtonAction_->setVisible(false);
  }
  setEditorActionsEnabled(false);
}

void MainWindow::showEditor()
{
  stack_->setCurrentWidget(editor_);

  if (toolbar_)
  {
    toolbar_->setVisible(true);
  }

  setEditorActionsEnabled(true);
  updateGitUiVisibility();
}

void MainWindow::setEditorActionsEnabled(bool enabled)
{
  if (saveProjectAction_)
  {
    saveProjectAction_->setEnabled(enabled);
  }

  if (addChildAction_)
  {
    addChildAction_->setEnabled(enabled);
  }

  if (removeItemAction_)
  {
    removeItemAction_->setEnabled(enabled);
  }

  if (goHomeAction_)
  {
    goHomeAction_->setEnabled(enabled);
  }

  if (projectMetadataAction_)
  {
    projectMetadataAction_->setEnabled(enabled);
  }

  if (gitInitAction_)
  {
    gitInitAction_->setEnabled(enabled);
  }

  if (gitStatusAction_)
  {
    gitStatusAction_->setEnabled(enabled);
  }

  if (gitCommitAction_)
  {
    gitCommitAction_->setEnabled(enabled);
  }

  if (gitConfigAction_)
  {
    gitConfigAction_->setEnabled(enabled);
  }
}

void MainWindow::updateWindowTitle()
{
  if (editor_ && editor_->project())
  {
    const QString name = editor_->project()->name().trimmed();
    if (!name.isEmpty())
    {
      setWindowTitle("CONFIGURED - " + name);
      return;
    }
  }

  setWindowTitle("CONFIGURED");
}

void MainWindow::promptAndCreateProject()
{
  editor_->createNewProject();

  ProjectMetadataDialog dialog(
      editor_->project(),
      QString(),
      &gitService_,
      this);

  if (dialog.exec() != QDialog::Accepted)
  {
    showHome();
    return;
  }

  if (!editor_->project())
  {
    showHome();
    return;
  }

  const QString projectName = editor_->project()->name().trimmed();
  if (projectName.isEmpty())
  {
    QMessageBox::warning(this, "New Project", "Project name cannot be empty.");
    showHome();
    return;
  }

  qDebug() << "Git managed:" << editor_->project()->isGitManaged();

  if (editor_->project()->isGitManaged())
  {
    const QString baseFolder = QFileDialog::getExistingDirectory(
        this,
        "Select Parent Folder for Git Project");

    if (baseFolder.isEmpty())
    {
      showHome();
      return;
    }

    QDir baseDir(baseFolder);
    const QString folderName = projectName;
    const QString projectFolderPath = baseDir.filePath(folderName);

    if (!baseDir.exists(folderName))
    {
      if (!baseDir.mkdir(folderName))
      {
        QMessageBox::warning(this, "Project Creation Failed",
                             "Could not create project folder.");
        showHome();
        return;
      }
    }

    const QString projectFilePath =
        QDir(projectFolderPath).filePath(projectName + ".configured");

    if (!editor_->saveProject(projectFilePath))
    {
      QMessageBox::warning(this, "Project Creation Failed",
                           "Could not save project file.");
      showHome();
      return;
    }

    if (!gitService_.isGitAvailable())
    {
      QMessageBox::warning(this, "Git", "Git not found. Please ensure Git is installed and available in the system PATH.");
      showHome();
      return;
    }

    QString output;
    if (!gitService_.initRepository(projectFolderPath, &output))
    {
      QMessageBox::warning(this, "Git Init Failed",
                           output.isEmpty() ? "Could not initialize Git repository." : output);
      showHome();
      return;
    }

    editor_->project()->setGitManaged(true);
  }

  updateWindowTitle();
  showEditor();

  XmlProjectExporter exporter;
  QString error;

  if (!exporter.exportParameters(*editor_->project(), "test.xml", &error))
  {
    qDebug() << "Export failed:" << error;
  }
  else
  {
    qDebug() << "Export success";
  }
}

QString MainWindow::currentProjectWorkingDirectory() const
{
  if (!editor_ || !editor_->hasProjectFilePath())
  {
    return QString();
  }

  QFileInfo info(editor_->currentFilePath());
  return info.absolutePath();
}

void MainWindow::updateGitUiVisibility()
{
  if (!gitButton_)
  {
    return;
  }

  bool showGit = false;

  if (editor_ && editor_->project())
  {
    showGit = editor_->project()->isGitManaged();

    if (showGit)
    {
      const QString workingDir = currentProjectWorkingDirectory();
      QString output;
      if (workingDir.isEmpty() || !gitService_.isGitAvailable(&output) || !gitService_.isRepository(workingDir, &output))
      {
        showGit = false;
      }
    }
  }
  if (gitButtonAction_)
  {
    gitButtonAction_->setVisible(showGit);
  }
}

void MainWindow::showHelp()
{
  stack_->setCurrentWidget(help_);

  if (toolbar_)
  {
    toolbar_->setVisible(true);
  }
}