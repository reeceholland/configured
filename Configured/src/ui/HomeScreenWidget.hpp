#pragma once

#include <QWidget>
#include "ConnectWindow.hpp"

class HomeScreenWidget : public QWidget
{
	Q_OBJECT
public:
	explicit HomeScreenWidget(QWidget* parent = nullptr);

private slots:
	void connectClicked();
	

signals:
	void connectSourceRequested();
	void createConfigRequested();
	void settingsRequested();
	void helpRequested();

private:
	ConnectWindow* connectWindow_ = nullptr;
};