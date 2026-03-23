#include "app/MainWindow.hpp"

#include <QStackedWidget>

#include "ui/HomeScreenWidget.hpp"
#include "ui/EditorScreenWidget.hpp"

MainWindow::MainWindow()
{
    setWindowTitle("CONFIGURED");
    resize(1200, 800);

    stack_ = new QStackedWidget(this);
    setCentralWidget(stack_);

    home_ = new HomeScreenWidget(this);
    editor_ = new EditorScreenWidget(this);

    stack_->addWidget(home_);
    stack_->addWidget(editor_);

    showHome();

    // Navigation
    connect(home_, &HomeScreenWidget::createRequested, this, [this]()
            { showEditor(); });
}

void MainWindow::showHome()
{
    stack_->setCurrentWidget(home_);
}

void MainWindow::showEditor()
{
    stack_->setCurrentWidget(editor_);
}