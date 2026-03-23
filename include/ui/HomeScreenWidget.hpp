#pragma once

#include <QWidget>

class QPushButton;

class HomeScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreenWidget(QWidget *parent = nullptr);

signals:
    void createRequested();

private:
    QPushButton *createBtn_ = nullptr;
};