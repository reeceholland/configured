#pragma once

#include <QDialog>

class QLineEdit;
class QTextEdit;
class QLabel;
class QCheckBox;
class ConfiguredProject;
class GitService;

class ProjectMetadataDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ProjectMetadataDialog(ConfiguredProject* project, const QString& workingDir,
                                 GitService* gitService, QWidget* parent = nullptr);

 private:
  void loadFromProject();
  void applyToProject();
  bool ensureGitRepository();

 private:
  ConfiguredProject* project_ = nullptr;
  QString workingDir_;
  GitService* gitService_ = nullptr;

  QLineEdit* nameEdit_ = nullptr;
  QTextEdit* descriptionEdit_ = nullptr;
  QLineEdit* authorEdit_ = nullptr;
  QLineEdit* companyEdit_ = nullptr;
  QLineEdit* versionEdit_ = nullptr;
  QLabel* lastModifiedLabel_ = nullptr;
  QLineEdit* robotPlatformEdit_ = nullptr;
  QCheckBox* gitManagedCheck_ = nullptr;
  QLabel* gitCommitLabel_ = nullptr;
};