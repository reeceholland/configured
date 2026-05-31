#include "ui/ProjectMetadataDialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "core/validation/ValidationMessage.hpp"
#include "core/validation/ValidationResult.hpp"
#include "core/validation/ValidationSeverity.hpp"
#include "core/validation/metadata/ProjectMetadataValidator.hpp"

ProjectMetadataDialog::ProjectMetadataDialog(const ProjectMetadata& initialValues, QWidget* parent)
    : QDialog(parent) {
  setWindowTitle("Project Metadata");
  resize(500, 400);

  auto* layout = new QVBoxLayout(this);
  auto* form = new QFormLayout();

  nameEdit_ = new QLineEdit(this);
  descriptionEdit_ = new QTextEdit(this);
  descriptionEdit_->setMinimumHeight(100);
  authorEdit_ = new QLineEdit(this);
  companyEdit_ = new QLineEdit(this);
  versionEdit_ = new QLineEdit(this);
  lastModifiedLabel_ = new QLabel(this);
  robotPlatformEdit_ = new QLineEdit(this);
  gitManagedCheck_ = new QCheckBox("Git Managed Configuration", this);
  gitCommitLabel_ = new QLabel(this);

  nameErrorLabel_ = new QLabel(this);
  nameErrorLabel_->setStyleSheet("color: #ff6b6b;");
  nameErrorLabel_->setVisible(false);

  lastModifiedLabel_->setStyleSheet("color: #aaaaaa;");
  gitCommitLabel_->setStyleSheet("color: #aaaaaa;");

  form->addRow("Project Name:", nameEdit_);
  form->addRow("", nameErrorLabel_);
  form->addRow("Description:", descriptionEdit_);
  form->addRow("Author:", authorEdit_);
  form->addRow("Company:", companyEdit_);
  form->addRow("Version:", versionEdit_);
  form->addRow("Last Modified:", lastModifiedLabel_);
  form->addRow("Platform:", robotPlatformEdit_);
  form->addRow("", gitManagedCheck_);
  form->addRow("Git Commit:", gitCommitLabel_);

  buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

  layout->addLayout(form);
  layout->addWidget(buttons_);

  loadFromMetadata(initialValues);

  connect(buttons_, &QDialogButtonBox::accepted, this, &ProjectMetadataDialog::validateAndAccept);
  connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);

  connect(nameEdit_, &QLineEdit::textChanged, this, &ProjectMetadataDialog::refreshValidationUi);
  connect(descriptionEdit_, &QTextEdit::textChanged, this,
          &ProjectMetadataDialog::refreshValidationUi);
  connect(authorEdit_, &QLineEdit::textChanged, this, &ProjectMetadataDialog::refreshValidationUi);
  connect(companyEdit_, &QLineEdit::textChanged, this, &ProjectMetadataDialog::refreshValidationUi);
  connect(versionEdit_, &QLineEdit::textChanged, this, &ProjectMetadataDialog::refreshValidationUi);
  connect(robotPlatformEdit_, &QLineEdit::textChanged, this,
          &ProjectMetadataDialog::refreshValidationUi);
  connect(gitManagedCheck_, &QCheckBox::stateChanged, this, [this](int) {
    refreshValidationUi();
  });

  refreshValidationUi();
}

void ProjectMetadataDialog::loadFromMetadata(const ProjectMetadata& metadata) {
  nameEdit_->setText(metadata.name);
  descriptionEdit_->setPlainText(metadata.description);
  authorEdit_->setText(metadata.author);
  companyEdit_->setText(metadata.company);
  versionEdit_->setText(metadata.version);
  robotPlatformEdit_->setText(metadata.robotPlatform);
  gitManagedCheck_->setChecked(metadata.gitManaged);
  lastModifiedLabel_->setText(metadata.lastModified.isEmpty() ? "—" : metadata.lastModified);
  gitCommitLabel_->setText(metadata.gitCommitHash.isEmpty() ? "—" : metadata.gitCommitHash);
}

ProjectMetadata ProjectMetadataDialog::metadata() const {
  ProjectMetadata metadata;
  metadata.name = nameEdit_->text().trimmed();
  metadata.description = descriptionEdit_->toPlainText().trimmed();
  metadata.author = authorEdit_->text().trimmed();
  metadata.company = companyEdit_->text().trimmed();
  metadata.version = versionEdit_->text().trimmed();
  metadata.robotPlatform = robotPlatformEdit_->text().trimmed();
  metadata.gitManaged = gitManagedCheck_->isChecked();
  metadata.lastModified = lastModifiedLabel_->text();
  metadata.gitCommitHash = gitCommitLabel_->text();
  return metadata;
}

void ProjectMetadataDialog::validateAndAccept() {
  const ProjectMetadata currentMetadata = metadata();

  ProjectMetadataValidator validator;
  const ValidationResult result = validator.validate(currentMetadata);

  if (!result.isValid()) {
    refreshValidationUi();
    return;
  }

  accept();
}

void ProjectMetadataDialog::refreshValidationUi() {
  const ProjectMetadata currentMetadata = metadata();

  ProjectMetadataValidator validator;
  const ValidationResult result = validator.validate(currentMetadata);

  const QString nameError = firstErrorForField(result, "name");
  const bool nameValid = nameError.isEmpty();

  setFieldValid(nameEdit_, nameValid);
  nameErrorLabel_->setText(nameError);
  nameErrorLabel_->setVisible(!nameValid);

  if (QPushButton* okButton = buttons_->button(QDialogButtonBox::Ok)) {
    okButton->setEnabled(result.isValid());
  }
}

void ProjectMetadataDialog::setFieldValid(QWidget* widget, bool valid) {
  if (valid) {
    widget->setStyleSheet("");
    return;
  }

  widget->setStyleSheet("border: 1px solid #ff6b6b;");
}

QString ProjectMetadataDialog::firstErrorForField(const ValidationResult& result,
                                                  const QString& field) const {
  for (const ValidationMessage& message : result.messages()) {
    if (message.field == field && message.severity == ValidationSeverity::Error) {
      return message.message;
    }
  }

  return "";
}
