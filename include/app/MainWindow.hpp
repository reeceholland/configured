#pragma once

#include <QMainWindow>

class QStackedWidget;
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

private:
    QStackedWidget *stack_ = nullptr;

    HomeScreenWidget *home_ = nullptr;
    EditorScreenWidget *editor_ = nullptr;
};