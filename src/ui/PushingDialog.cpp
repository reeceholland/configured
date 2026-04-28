#include "ui/PushingDialog.hpp"

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

PushingDialog::PushingDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Pushing Changes");
  setModal(true);

  auto* layout = new QVBoxLayout(this);

  statusLabel_ = new QLabel("Pushing changes...", this);

  outputEdit_ = new QTextEdit(this);
  outputEdit_->setReadOnly(true);
  outputEdit_->setMinimumSize(QSize(400, 200));

  cancelButton_ = new QPushButton("Cancel", this);

  layout->addWidget(statusLabel_);
  layout->addWidget(outputEdit_);
  layout->addWidget(cancelButton_);

  connect(cancelButton_, &QPushButton::clicked, this, &PushingDialog::cancelRequested);
}

void PushingDialog::setStatusText(const QString& text) {
  statusLabel_->setText(text);
}

void PushingDialog::appendOutput(const QString& text) {
  if (!text.trimmed().isEmpty()) {
    outputEdit_->append(text.trimmed());
  }
}