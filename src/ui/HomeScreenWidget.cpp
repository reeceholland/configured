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

    createBtn_ = new QPushButton("New Project", this);
    createBtn_->setFixedSize(200, 40);

    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(createBtn_, 0, Qt::AlignCenter);

    connect(createBtn_, &QPushButton::clicked, this, &HomeScreenWidget::createRequested);
}