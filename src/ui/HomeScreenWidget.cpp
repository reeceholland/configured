#include "ui/HomeScreenWidget.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

HomeScreenWidget::HomeScreenWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    auto *title = new QLabel("CONFIGURED", this);
    QFont tf = title->font();
    tf.setPointSize(28);
    tf.setBold(true);
    title->setFont(tf);
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel("Robotics Configuration Studio", this);
    subtitle->setAlignment(Qt::AlignCenter);

    createNewProjectBtn_ = new QPushButton("New Project", this);
    createNewProjectBtn_->setFixedSize(200, 40);

    openProjectBtn_ = new QPushButton("Open Project", this);
    openProjectBtn_->setFixedSize(200, 40);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(createNewProjectBtn_, 0, Qt::AlignCenter);
    layout->addWidget(openProjectBtn_, 0, Qt::AlignCenter);

    connect(createNewProjectBtn_, &QPushButton::clicked, this, &HomeScreenWidget::createNewProjectRequested);
    connect(openProjectBtn_, &QPushButton::clicked, this, &HomeScreenWidget::openProjectRequested);
}