#pragma once

#include <QDialog>

class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @brief Dialog that collects clone/connect details for a remote repository.
 */
class RemoteConnectDialog : public QDialog {
  Q_OBJECT

 public:
  /// Create a remote connection dialog.
  explicit RemoteConnectDialog(QWidget* parent = nullptr);

  /// Repository URL entered by the user.
  QString remoteUrl() const;

  /// Parent folder selected as the clone destination.
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
