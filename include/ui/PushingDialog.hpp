#pragma once

#include <QDialog>

class QLabel;
class QPushButton;
class QTextEdit;

class PushingDialog : public QDialog {
  Q_OBJECT
 public:
  explicit PushingDialog(QWidget* parent = nullptr);

  void setStatusText(const QString& text);
  void appendOutput(const QString& text);

 signals:

  void cancelRequested();

 private:
  QLabel* statusLabel_ = nullptr;
  QTextEdit* outputEdit_ = nullptr;
  QPushButton* cancelButton_ = nullptr;
};