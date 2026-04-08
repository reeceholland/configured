#pragma once

#include <QMainWindow>

class QStackedWidget;
class QAction;
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

private:
    QStackedWidget *stack_ = nullptr;

    HomeScreenWidget *home_ = nullptr;
    EditorScreenWidget *editor_ = nullptr;

    QToolBar *toolbar_ = nullptr;

    QAction *saveProjectAction_ = nullptr;
    QAction *addChildAction_ = nullptr;
    QAction *removeItemAction_ = nullptr;
    QAction *goHomeAction_ = nullptr;
    QAction *projectMetadataAction_ = nullptr;
};