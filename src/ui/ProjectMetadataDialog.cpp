#include "ui/ProjectMetadataDialog.hpp"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>

#include "core/ConfiguredProject.hpp"
#include "core/GitService.hpp"

ProjectMetadataDialog::ProjectMetadataDialog(ConfiguredProject *project,
                                             const QString &workingDir,
                                             GitService *gitService,
                                             QWidget *parent)
    : QDialog(parent),
      project_(project),
      workingDir_(workingDir),
      gitService_(gitService)
{
    setWindowTitle("Project Metadata");
    resize(500, 400);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

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
    form->addRow("Robot Platform:", robotPlatformEdit_);
    form->addRow("", gitManagedCheck_);
    form->addRow("Git Commit:", gitCommitLabel_);

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this);

    layout->addLayout(form);
    layout->addWidget(buttons);

    loadFromProject();

    connect(buttons, &QDialogButtonBox::accepted, this, [this]()
            {
        applyToProject();
        accept(); });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProjectMetadataDialog::loadFromProject()
{
    if (!project_)
    {
        return;
    }

    nameEdit_->setText(project_->name());
    descriptionEdit_->setPlainText(project_->description());
    authorEdit_->setText(project_->author());
    companyEdit_->setText(project_->company());
    versionEdit_->setText(project_->version());
    robotPlatformEdit_->setText(project_->robotPlatform());
    gitManagedCheck_->setChecked(project_->isGitManaged());
    lastModifiedLabel_->setText(project_->lastModified());

    QString commitHash = "-";

    if (project_->isGitManaged() && gitService_ && !workingDir_.trimmed().isEmpty())
    {
        QString output;
        if (gitService_->getCommitHash(workingDir_, &commitHash, &output))
        {
            if (commitHash.trimmed().isEmpty())
            {
                commitHash = "-";
            }
        }
        else
        {
            commitHash = "No commits";
        }
    }

    gitCommitLabel_->setText(commitHash);
}

void ProjectMetadataDialog::applyToProject()
{
    if (!project_)
    {
        return;
    }

    project_->setName(nameEdit_->text().trimmed());
    project_->setDescription(descriptionEdit_->toPlainText().trimmed());
    project_->setAuthor(authorEdit_->text().trimmed());
    project_->setCompany(companyEdit_->text().trimmed());
    project_->setVersion(versionEdit_->text().trimmed());
    project_->setRobotPlatform(robotPlatformEdit_->text().trimmed());
    project_->setGitManaged(gitManagedCheck_->isChecked());
}