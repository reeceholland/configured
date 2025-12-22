#pragma once

#include <QWidget>

class HomeScreenWidget : public QWidget
{
	Q_OBJECT
public:
	explicit HomeScreenWidget(QWidget* parent = nullptr);

signals:
	void connectSourceRequested();
	void createConfigRequested();
	void settingsRequested();
	void helpRequested();
};