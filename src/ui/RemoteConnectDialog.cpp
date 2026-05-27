#include "ui/RemoteConnectDialog.hpp"

#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

RemoteConnectDialog::RemoteConnectDialog(QWidget* parent) : QDialog(parent) {
  setWindowTitle("Connect Remote Project");
  setModal(true);

  auto* layout = new QVBoxLayout(this);

  auto* form = new QFormLayout();

  remoteUrlEdit_ = new QLineEdit(this);
  remoteUrlEdit_->setPlaceholderText("git@github.com:user/project.git");

  remoteUrlErrorLabel_ = new QLabel(this);
  remoteUrlErrorLabel_->setStyleSheet("color: #ff6b6b;");
  remoteUrlErrorLabel_->setVisible(false);

  parentFolderEdit_ = new QLineEdit(this);
  parentFolderEdit_->setPlaceholderText("Select a local parent folder");

  browseButton_ = new QPushButton("Browse", this);

  auto* folderRow = new QWidget(this);
  auto* folderLayout = new QHBoxLayout(folderRow);
  folderLayout->setContentsMargins(0, 0, 0, 0);
  folderLayout->addWidget(parentFolderEdit_);
  folderLayout->addWidget(browseButton_);

  parentFolderErrorLabel_ = new QLabel(this);
  parentFolderErrorLabel_->setStyleSheet("color: #ff6b6b;");
  parentFolderErrorLabel_->setVisible(false);

  form->addRow("Repository URL:", remoteUrlEdit_);
  form->addRow("", remoteUrlErrorLabel_);
  form->addRow("Parent Folder:", folderRow);
  form->addRow("", parentFolderErrorLabel_);

  buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  buttons_->button(QDialogButtonBox::Ok)->setText("Connect");

  layout->addLayout(form);
  layout->addWidget(buttons_);

  connect(browseButton_, &QPushButton::clicked, this, &RemoteConnectDialog::browseForParentFolder);

  connect(remoteUrlEdit_, &QLineEdit::textChanged, this,
          &RemoteConnectDialog::updateValidationState);

  connect(parentFolderEdit_, &QLineEdit::textChanged, this,
          &RemoteConnectDialog::updateValidationState);

  connect(buttons_, &QDialogButtonBox::accepted, this, [this]() {
    updateValidationState();

    if (remoteUrl().isEmpty() || parentFolder().isEmpty() || !QDir(parentFolder()).exists()) {
      return;
    }

    accept();
  });

  connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);

  updateValidationState();
}

QString RemoteConnectDialog::remoteUrl() const {
  return remoteUrlEdit_ ? remoteUrlEdit_->text().trimmed() : QString();
}

QString RemoteConnectDialog::parentFolder() const {
  return parentFolderEdit_ ? parentFolderEdit_->text().trimmed() : QString();
}

void RemoteConnectDialog::browseForParentFolder() {
  const QString folder =
      QFileDialog::getExistingDirectory(this, "Select Parent Folder", parentFolder());

  if (folder.isEmpty()) {
    return;
  }

  parentFolderEdit_->setText(folder);
}

void RemoteConnectDialog::updateValidationState() {
  const bool hasRemoteUrl = !remoteUrl().isEmpty();
  const bool hasParentFolder = !parentFolder().isEmpty();
  const bool parentFolderExists = hasParentFolder && QDir(parentFolder()).exists();

  remoteUrlErrorLabel_->setText(hasRemoteUrl ? "" : "Repository URL is required.");
  remoteUrlErrorLabel_->setVisible(!hasRemoteUrl);

  parentFolderErrorLabel_->setText(
      hasParentFolder ? "Parent folder does not exist." : "Parent folder is required.");
  parentFolderErrorLabel_->setVisible(!parentFolderExists);

  if (buttons_) {
    buttons_->button(QDialogButtonBox::Ok)->setEnabled(hasRemoteUrl && parentFolderExists);
  }
}
