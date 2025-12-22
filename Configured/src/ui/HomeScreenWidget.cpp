#include "ui/HomeScreenWidget.hpp"

#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QIcon>
#include <QSize>

HomeScreenWidget::HomeScreenWidget(QWidget* parent)
{
	auto* outer = new QVBoxLayout(this);
	outer->setContentsMargins(80, 80, 80, 80);
	outer->setSpacing(24);
}