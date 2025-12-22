#include "app/MainWindow.hpp"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QStackedWidget>

MainWindow::MainWindow()
{
	this->setWindowTitle("CONFIGURED");
	this->resize(1200, 800);

	stack_ = new QStackedWidget(this);
	this->setCentralWidget(stack_);

	home_ = new HomeScreenWidget(stack_);
	stack_->addWidget(home_);
	stack_->setCurrentWidget(home_);
}