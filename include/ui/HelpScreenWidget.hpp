#pragma once

#include <QWidget>

class QPushButton;
class QTextBrowser;

class HelpScreenWidget : public QWidget {
  Q_OBJECT

 public:
  explicit HelpScreenWidget(QWidget* parent = nullptr);

 signals:
  void backRequested();

 private:
  QTextBrowser* helpText_ = nullptr;
  QPushButton* backButton_ = nullptr;
};