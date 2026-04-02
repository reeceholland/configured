#pragma once

#include <QWidget>

class QPushButton;

class HomeScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreenWidget(QWidget *parent = nullptr);

signals:
    void createNewProjectRequested();
    void openProjectRequested();

private:
    QPushButton *createNewProjectBtn_ = nullptr;
    QPushButton *openProjectBtn_ = nullptr;
};