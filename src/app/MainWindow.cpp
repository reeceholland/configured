#include "app/MainWindow.hpp"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QStackedWidget>
#include <QToolBar>
#include <QInputDialog>

#include "ui/HomeScreenWidget.hpp"
#include "ui/EditorScreenWidget.hpp"

MainWindow::MainWindow()
{
  // Set up the main window
  setWindowTitle("CONFIGURED");
  resize(800, 400);

  // Create the stacked widget to hold the home and editor screens
  stack_ = new QStackedWidget(this);
  setCentralWidget(stack_);

  // Create the home and editor screens
  home_ = new HomeScreenWidget(this);
  editor_ = new EditorScreenWidget(this);

  // Add the screens to the stacked widget
  stack_->addWidget(home_);
  stack_->addWidget(editor_);

  // Create the toolbar
  toolbar_ = addToolBar("Main");
  toolbar_->setMovable(false);

  // Add actions to the toolbar
  saveProjectAction_ = toolbar_->addAction("Save Project");
  addChildAction_ = toolbar_->addAction("Add Child");
  removeItemAction_ = toolbar_->addAction("Remove Selected");
  goHomeAction_ = toolbar_->addAction("Home");

  connect(saveProjectAction_, &QAction::triggered, this, [this]()
          {
        const QString filePath = QFileDialog::getSaveFileName(
            this,
            "Save Configured Project",
            QString(),
            "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)"
        );

        if (filePath.isEmpty()) {
            return;
        }

        if (!editor_->saveProject(filePath)) {
            QMessageBox::warning(this, "Save Failed", "Could not save project file.");
        } });

  connect(addChildAction_, &QAction::triggered, this, [this]()
          { editor_->addChildToSelected(); });

  connect(removeItemAction_, &QAction::triggered, this, [this]()
          { editor_->removeSelectedItem(); });

  connect(goHomeAction_, &QAction::triggered, this, [this]()
          { showHome(); });

  connect(home_, &HomeScreenWidget::createNewProjectRequested, this, [this]()
          {
            bool ok = false;
            QString name = QInputDialog::getText(this, "New Project", "Enter project name:", QLineEdit::Normal, "Untitled Project", &ok);

            if (!ok || name.trimmed().isEmpty())
            {
              return;
            } 
            editor_->createNewProject();
            showEditor();
            editor_->setProjectName(name); });

  connect(home_, &HomeScreenWidget::openProjectRequested, this, [this]()
          {
        const QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open Configured Project",
            QString(),
            "Configured Project (*.configured);;JSON Files (*.json);;All Files (*)"
        );

        if (filePath.isEmpty()) {
            return;
        }

        if (!editor_->loadProject(filePath)) {
            QMessageBox::warning(this, "Open Failed", "Could not load project file.");
            return;
        }

        showEditor(); });

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
}