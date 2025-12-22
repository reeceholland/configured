#pragma once

#include "ui/HomeScreenWidget.hpp"

#include <QMainWindow>

class QStackedWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();

private:
	QStackedWidget* stack_ = nullptr;
	HomeScreenWidget* home_ = nullptr;
};