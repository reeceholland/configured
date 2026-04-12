#include "ui/GitConfigDialog.hpp"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

#include "core/GitService.hpp"

GitConfigDialog::GitConfigDialog(GitService *gitService,
                                 const QString &workingDir,
                                 QWidget *parent)
    : QDialog(parent),
      gitService_(gitService),
      workingDir_(workingDir)
{
    setWindowTitle("Git Identity");
    resize(420, 220);

    auto *mainLayout = new QVBoxLayout(this);

    auto *scopeGroup = new QGroupBox("Scope", this);
    auto *scopeLayout = new QHBoxLayout(scopeGroup);

    globalRadio_ = new QRadioButton("Global", scopeGroup);
    localRadio_ = new QRadioButton("This Repository", scopeGroup);

    globalRadio_->setChecked(true);

    scopeLayout->addWidget(globalRadio_);
    scopeLayout->addWidget(localRadio_);

    auto *form = new QFormLayout();
    nameEdit_ = new QLineEdit(this);
    emailEdit_ = new QLineEdit(this);

    form->addRow("Name:", nameEdit_);
    form->addRow("Email:", emailEdit_);

    auto *buttonRow = new QHBoxLayout();
    auto *loadButton = new QPushButton("Load Current", this);
    buttonRow->addWidget(loadButton);
    buttonRow->addStretch();

    auto *buttons = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel,
        this);

    mainLayout->addWidget(scopeGroup);
    mainLayout->addLayout(form);
    mainLayout->addLayout(buttonRow);
    mainLayout->addWidget(buttons);

    connect(loadButton, &QPushButton::clicked, this, [this]()
            { loadCurrentValues(); });

    connect(globalRadio_, &QRadioButton::toggled, this, [this](bool)
            { loadCurrentValues(); });

    connect(buttons, &QDialogButtonBox::accepted, this, [this]()
            { saveValues(); });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    loadCurrentValues();
}

void GitConfigDialog::loadCurrentValues()
{
    if (!gitService_)
    {
        return;
    }

    const bool global = globalRadio_->isChecked();

    QString name;
    QString email;
    QString output;

    gitService_->getConfigValue(workingDir_, "user.name", global, &name, &output);
    gitService_->getConfigValue(workingDir_, "user.email", global, &email, &output);

    nameEdit_->setText(name);
    emailEdit_->setText(email);
}

void GitConfigDialog::saveValues()
{
    if (!gitService_)
    {
        reject();
        return;
    }

    const bool global = globalRadio_->isChecked();
    const QString name = nameEdit_->text().trimmed();
    const QString email = emailEdit_->text().trimmed();

    if (name.isEmpty() || email.isEmpty())
    {
        QMessageBox::warning(this, "Git Identity", "Name and email must both be provided.");
        return;
    }

    QString output;

    if (!gitService_->setConfigValue(workingDir_, "user.name", name, global, &output))
    {
        QMessageBox::warning(this, "Git Identity", "Failed to set user.name\n\n" + output);
        return;
    }

    if (!gitService_->setConfigValue(workingDir_, "user.email", email, global, &output))
    {
        QMessageBox::warning(this, "Git Identity", "Failed to set user.email\n\n" + output);
        return;
    }

    accept();
}