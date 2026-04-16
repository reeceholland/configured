#include "ui/ProjectMetadataDialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextEdit>
#include <QVBoxLayout>

#include "core/ConfiguredProject.hpp"
#include "core/GitService.hpp"

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

  lastModifiedLabel_->setStyleSheet("color: #aaaaaa;");
  gitCommitLabel_->setStyleSheet("color: #aaaaaa;");

  form->addRow("Project Name:", nameEdit_);
  form->addRow("Description:", descriptionEdit_);
  form->addRow("Author:", authorEdit_);
  form->addRow("Company:", companyEdit_);
  form->addRow("Version:", versionEdit_);
  form->addRow("Last Modified:", lastModifiedLabel_);
  form->addRow("Platform:", robotPlatformEdit_);
  form->addRow("", gitManagedCheck_);
  form->addRow("Git Commit:", gitCommitLabel_);

  auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

  layout->addLayout(form);
  layout->addWidget(buttons);

  loadFromMetadata(initialValues);

  connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
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
