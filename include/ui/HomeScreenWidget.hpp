#pragma once

#include <QWidget>
#include <QPixmap>

class QPushButton;

class HomeScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreenWidget(QWidget *parent = nullptr);

signals:
    void createNewProjectRequested();
    void openProjectRequested();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPushButton *createNewProjectBtn_ = nullptr;
    QPushButton *openProjectBtn_ = nullptr;
    QPixmap background_;
};