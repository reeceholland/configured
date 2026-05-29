#include "ui/HelpScreenWidget.hpp"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>

HelpScreenWidget::HelpScreenWidget(QWidget* parent) : QWidget(parent) {
  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(24, 24, 24, 24);
  rootLayout->setSpacing(16);

  auto* title = new QLabel("Help", this);
  QFont titleFont = title->font();
  titleFont.setPointSize(20);
  titleFont.setBold(true);
  title->setFont(titleFont);

  helpText_ = new QTextBrowser(this);
  helpText_->setOpenExternalLinks(true);

  QFile file(":/help/help.html");
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    const QString html = QString::fromUtf8(file.readAll());
    helpText_->setHtml(html);
  } else {
    helpText_->setHtml("<h2>Help unavailable</h2><p>Could not load help content.</p>");
  }

  backButton_ = new QPushButton("Back", this);
  connect(backButton_, &QPushButton::clicked, this, &HelpScreenWidget::backRequested);

  rootLayout->addWidget(title);
  rootLayout->addWidget(helpText_, 1);
  rootLayout->addWidget(backButton_);
}