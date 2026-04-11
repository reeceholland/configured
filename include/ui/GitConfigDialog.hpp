#pragma once

#include <QDialog>

class QLineEdit;
class QRadioButton;
class GitService;

class GitConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GitConfigDialog(GitService *gitService,
                             const QString &workingDir,
                             QWidget *parent = nullptr);

private:
    void loadCurrentValues();
    void saveValues();

private:
    GitService *gitService_ = nullptr;
    QString workingDir_;

    QLineEdit *nameEdit_ = nullptr;
    QLineEdit *emailEdit_ = nullptr;
    QRadioButton *globalRadio_ = nullptr;
    QRadioButton *localRadio_ = nullptr;
};