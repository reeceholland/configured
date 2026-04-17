#pragma once

#include <QDialog>

#include "core/ProjectMetadata.hpp"

class QLineEdit;
class QTextEdit;
class QLabel;
class QCheckBox;
class QDialogButtonBox;
class ConfiguredProject;
class GitService;

class ProjectMetadataDialog : public QDialog {
  Q_OBJECT

 public:
  explicit ProjectMetadataDialog(const ProjectMetadata& initialValues, QWidget* parent);

  ProjectMetadata metadata() const;

 private:
  void loadFromMetadata(const ProjectMetadata& metadata);
  void refreshValidationUi();
  void setFieldValid(QWidget* widget, bool valid);
  QString firstErrorForField(const class ValidationResult& result, const QString& field) const;

 private slots:
  void validateAndAccept();

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

  QLabel* nameErrorLabel_{nullptr};

  QDialogButtonBox* buttons_{nullptr};
};