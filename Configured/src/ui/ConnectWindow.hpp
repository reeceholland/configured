#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class ConnectWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectWindow(QWidget* parent = nullptr);
    private slots:
    void onLocalConfiguredFileClicked();

private:
    QLabel* title_ = nullptr;
    QPushButton* closeBtn_ = nullptr;
};
