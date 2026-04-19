#pragma once

#include <QDialog>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;

class RemoteConnectDialog : public QDialog {
  Q_OBJECT

 public:
  explicit RemoteConnectDialog(QWidget* parent = nullptr);

  QString remoteUrl() const;
  QString parentFolder() const;

 private:
  void browseForParentFolder();
  void updateValidationState();

  QLineEdit* remoteUrlEdit_ = nullptr;
  QLineEdit* parentFolderEdit_ = nullptr;
  QPushButton* browseButton_ = nullptr;
  QLabel* remoteUrlErrorLabel_ = nullptr;
  QLabel* parentFolderErrorLabel_ = nullptr;
  QDialogButtonBox* buttons_ = nullptr;
};