#pragma once

#include <QDialog>
#include <QCheckBox>

class QLineEdit;
class QTextEdit;
class ConfiguredProject;

class ProjectMetadataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectMetadataDialog(ConfiguredProject *project, QWidget *parent = nullptr);

private:
    void loadFromProject();
    void applyToProject();

    ConfiguredProject *project_ = nullptr;
    QLineEdit *nameEdit_ = nullptr;
    QTextEdit *descriptionEdit_ = nullptr;
    QLineEdit *authorEdit_ = nullptr;
    QLineEdit *companyEdit_ = nullptr;
    QLineEdit *versionEdit_ = nullptr;
    QLineEdit *robotPlatformEdit_ = nullptr;
    QCheckBox *gitManagedCheck_ = nullptr;
};