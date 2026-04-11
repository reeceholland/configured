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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private:
    void showHome();
    void showEditor();
    void setEditorActionsEnabled(bool enabled);
    void updateWindowTitle();
    void promptAndCreateProject();
    void updateGitUiVisibility();
    QString currentProjectWorkingDirectory() const;

private:
    QStackedWidget *stack_ = nullptr;

    HomeScreenWidget *home_ = nullptr;
    EditorScreenWidget *editor_ = nullptr;

    QToolBar *toolbar_ = nullptr;
    QToolButton *gitButton_ = nullptr;

    QAction *saveProjectAction_ = nullptr;
    QAction *addChildAction_ = nullptr;
    QAction *removeItemAction_ = nullptr;
    QAction *goHomeAction_ = nullptr;
    QAction *projectMetadataAction_ = nullptr;

    QAction *gitInitAction_ = nullptr;
    QAction *gitStatusAction_ = nullptr;
    QAction *gitCommitAction_ = nullptr;
    QAction *gitConfigAction_ = nullptr;

    GitService gitService_;
};