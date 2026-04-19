#include "ui/ConnectingDialog.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

ConnectingDialog::ConnectingDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Connecting Remote Project");
  setModal(true);

  auto* layout = new QVBoxLayout(this);

  statusLabel_ = new QLabel("Cloning remote repository...", this);

  outputEdit_ = new QTextEdit(this);
  outputEdit_->setReadOnly(true);
  outputEdit_->setMinimumSize(QSize(400, 200));

  cancelButton_ = new QPushButton("Cancel", this);

  layout->addWidget(statusLabel_);
  layout->addWidget(outputEdit_);
  layout->addWidget(cancelButton_);

  connect(cancelButton_, &QPushButton::clicked, this, &ConnectingDialog::cancelRequested);
}

void ConnectingDialog::setStatusText(const QString& text) {
  statusLabel_->setText(text);
}

void ConnectingDialog::appendOutput(const QString& text) {
  if (!text.trimmed().isEmpty()) {
    outputEdit_->append(text.trimmed());
  }
}
