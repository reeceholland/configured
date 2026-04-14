#pragma once

#include <QMainWindow>

#include "core/GitService.hpp"

class QStackedWidget;
class QAction;
class QToolBar;
class QToolButton;
class QMenu;
class HomeScreenWidget;
class EditorScreenWidget;
class HelpScreenWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow();

 private:
  void showHome();
  void showEditor();
  void showHelp();
  void setEditorActionsEnabled(bool enabled);
  void updateWindowTitle();
  void promptAndCreateProject();
  void updateGitUiVisibility();
  QString currentProjectWorkingDirectory() const;
  void exportParametersToJson();

 private:
  QStackedWidget* stack_ = nullptr;

  HomeScreenWidget* home_ = nullptr;
  EditorScreenWidget* editor_ = nullptr;
  HelpScreenWidget* help_ = nullptr;

  QToolBar* toolbar_ = nullptr;
  QToolButton* gitButton_ = nullptr;

  QAction* saveProjectAction_ = nullptr;
  QAction* addChildAction_ = nullptr;
  QAction* removeItemAction_ = nullptr;
  QAction* goHomeAction_ = nullptr;
  QAction* projectMetadataAction_ = nullptr;
  QAction* versionAction_ = nullptr;
  QAction* helpAction_ = nullptr;
  QAction* gitButtonAction_ = nullptr;
  QAction* exportXmlAction_ = nullptr;
  QAction* exportJsonAction_ = nullptr;

  QAction* gitInitAction_ = nullptr;
  QAction* gitStatusAction_ = nullptr;
  QAction* gitCommitAction_ = nullptr;
  QAction* gitConfigAction_ = nullptr;

  QAction* exportAction_ = nullptr;

  GitService gitService_;
};