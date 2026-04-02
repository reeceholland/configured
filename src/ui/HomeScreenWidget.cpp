#include "ui/HomeScreenWidget.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>

HomeScreenWidget::HomeScreenWidget(QWidget *parent)
    : QWidget(parent)
{
    background_ = QPixmap(":/images/home_background.jpg");

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setAlignment(Qt::AlignCenter);
    rootLayout->setContentsMargins(40, 40, 40, 40);

    // Glass card container
    auto *card = new QWidget(this);
    card->setFixedWidth(400);
    card->setStyleSheet(R"(
        background-color: rgba(0, 0, 0, 140);
        border-radius: 16px;
    )");

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(30, 30, 30, 30);

    auto *title = new QLabel("CONFIGURED", card);
    QFont tf = title->font();
    tf.setPointSize(28);
    tf.setBold(true);
    title->setFont(tf);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; background: transparent;");

    auto *subtitle = new QLabel("Robotics Configuration Studio", card);
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #dddddd; background: transparent;");

    createNewProjectBtn_ = new QPushButton("New Project", card);
    createNewProjectBtn_->setFixedSize(200, 40);

    openProjectBtn_ = new QPushButton("Open Project", card);
    openProjectBtn_->setFixedSize(200, 40);

    cardLayout->addWidget(title);
    cardLayout->addWidget(subtitle);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(createNewProjectBtn_, 0, Qt::AlignCenter);
    cardLayout->addWidget(openProjectBtn_, 0, Qt::AlignCenter);

    rootLayout->addWidget(card, 0, Qt::AlignCenter);

    connect(createNewProjectBtn_, &QPushButton::clicked,
            this, &HomeScreenWidget::createNewProjectRequested);
    connect(openProjectBtn_, &QPushButton::clicked,
            this, &HomeScreenWidget::openProjectRequested);
}

void HomeScreenWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    if (!background_.isNull())
    {
        QPixmap scaled = background_.scaled(
            size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation);

        int x = (scaled.width() - width()) / 2;
        int y = (scaled.height() - height()) / 2;

        painter.drawPixmap(-x, -y, scaled);
    }

    // Dark overlay
    painter.fillRect(rect(), QColor(0, 0, 0, 120));
}