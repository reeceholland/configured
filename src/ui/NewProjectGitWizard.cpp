#include "ui/NewProjectGitWizard.hpp"

#include <QBrush>
#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWizardPage>

#include "core/git/GitService.hpp"

namespace {

enum WizardPageId { IntroPageId, IdentityPageId, CommitPageId, RemotePageId, PushPageId };

class IntroPage : public QWizardPage {
 public:
  explicit IntroPage(QWidget* parent = nullptr) : QWizardPage(parent) {
    setTitle("Git Setup");
    setSubTitle("Configure Git for your newly created project.");

    auto* layout = new QVBoxLayout(this);

    auto* label = new QLabel(
        "The project was created successfully and Git has been initialized.\n\n"
        "This wizard can help you configure identity, create an initial commit, "
        "connect a remote repository, and push your branch.",
        this);
    label->setWordWrap(true);
    layout->addWidget(label);
  }
};

class IdentityPage : public QWizardPage {
 public:
  explicit IdentityPage(QWidget* parent = nullptr) : QWizardPage(parent) {
    setTitle("Git Identity");
    setSubTitle("Configure your Git identity for this project.");

    auto* layout = new QVBoxLayout(this);

    auto* form = new QFormLayout();

    nameEdit_ = new QLineEdit(this);
    emailEdit_ = new QLineEdit(this);
    useGlobalCheck_ = new QCheckBox("Use global Git identity", this);

    registerField("userName*", nameEdit_);
    registerField("userEmail*", emailEdit_);
    registerField("useGlobalIdentity", useGlobalCheck_);

    form->addRow("Name:", nameEdit_);
    form->addRow("Email:", emailEdit_);

    layout->addLayout(form);
    layout->addWidget(useGlobalCheck_);
    layout->addStretch();
  }

 private:
  QLineEdit* nameEdit_ = nullptr;
  QLineEdit* emailEdit_ = nullptr;
  QCheckBox* useGlobalCheck_ = nullptr;
};

class CommitPage : public QWizardPage {
 public:
  explicit CommitPage(QWidget* parent = nullptr) : QWizardPage(parent) {
    setTitle("Initial Commit");
    setSubTitle("Choose whether to create the first commit now.");

    auto* layout = new QVBoxLayout(this);

    createCommitCheck_ = new QCheckBox("Create initial commit.", this);
    createCommitCheck_->setChecked(true);

    commitMessageEdit_ = new QLineEdit(this);
    commitMessageEdit_->setText("Initial configured project");

    registerField("createInitialCommit", createCommitCheck_);
    registerField("commitMessage", commitMessageEdit_);

    auto* form = new QFormLayout();
    form->addRow("Commit message:", commitMessageEdit_);

    layout->addWidget(createCommitCheck_);
    layout->addLayout(form);
    layout->addStretch();

    connect(createCommitCheck_, &QCheckBox::toggled, this, [this](bool checked) {
      commitMessageEdit_->setEnabled(checked);
      emit completeChanged();
    });
  }

  bool isComplete() const override {
    if (!createCommitCheck_->isChecked()) {
      return true;
    }
    return !commitMessageEdit_->text().trimmed().isEmpty();
  }

 private:
  QCheckBox* createCommitCheck_ = nullptr;
  QLineEdit* commitMessageEdit_ = nullptr;
};

class RemotePage : public QWizardPage {
 public:
  explicit RemotePage(QWidget* parent = nullptr) : QWizardPage(parent) {
    setTitle("Remote Repository");
    setSubTitle("Optionally connect a remote repository.");

    auto* layout = new QVBoxLayout(this);

    connectRemoteCheck_ = new QCheckBox("Connect remote repository", this);
    connectRemoteCheck_->setChecked(false);

    remoteNameEdit_ = new QLineEdit(this);
    remoteNameEdit_->setText("origin");

    remoteUrlEdit_ = new QLineEdit(this);

    registerField("connectRemote", connectRemoteCheck_);
    registerField("remoteName", remoteNameEdit_);
    registerField("remoteUrl", remoteUrlEdit_);

    auto* form = new QFormLayout();
    form->addRow("Remote name:", remoteNameEdit_);
    form->addRow("Remote URL:", remoteUrlEdit_);

    layout->addWidget(connectRemoteCheck_);
    layout->addLayout(form);
    layout->addStretch();

    const auto updateEnabledState = [this]() {
      const bool enabled = connectRemoteCheck_->isChecked();
      remoteNameEdit_->setEnabled(enabled);
      remoteUrlEdit_->setEnabled(enabled);
      emit completeChanged();
    };

    connect(connectRemoteCheck_, &QCheckBox::toggled, this, [updateEnabledState](bool) {
      updateEnabledState();
    });

    connect(remoteNameEdit_, &QLineEdit::textChanged, this, [this](const QString&) {
      emit completeChanged();
    });

    connect(remoteUrlEdit_, &QLineEdit::textChanged, this, [this](const QString&) {
      emit completeChanged();
    });

    updateEnabledState();
  }

  bool isComplete() const override {
    if (!connectRemoteCheck_->isChecked()) {
      return true;
    }

    return !remoteNameEdit_->text().trimmed().isEmpty()
           && !remoteUrlEdit_->text().trimmed().isEmpty();
  }

 private:
  QCheckBox* connectRemoteCheck_ = nullptr;
  QLineEdit* remoteNameEdit_ = nullptr;
  QLineEdit* remoteUrlEdit_ = nullptr;
};

class PushPage : public QWizardPage {
 public:
  explicit PushPage(QWidget* parent = nullptr) : QWizardPage(parent) {
    setTitle("First Push");
    setSubTitle("Choose whether to push the branch after setup.");

    auto* layout = new QVBoxLayout(this);

    pushAfterSetupCheck_ = new QCheckBox("Push branch after setup", this);
    registerField("pushAfterSetup", pushAfterSetupCheck_);

    auto* note = new QLabel(
        "This is only useful if a remote repository is configured and an initial commit exists.",
        this);
    note->setWordWrap(true);

    layout->addWidget(pushAfterSetupCheck_);
    layout->addWidget(note);
    layout->addStretch();
  }

 private:
  QCheckBox* pushAfterSetupCheck_ = nullptr;
};

}  // namespace

NewProjectGitWizard::NewProjectGitWizard(const QString& workingDir, GitService* gitService,
                                         QWidget* parent)
    : QWizard(parent), gitService_(gitService) {
  setWindowTitle("New Project Git Setup");
  setOption(QWizard::NoBackButtonOnStartPage, true);
  setWizardStyle(QWizard::ModernStyle);

  setPixmap(QWizard::WatermarkPixmap, QPixmap());
  setPixmap(QWizard::LogoPixmap, QPixmap());
  setPixmap(QWizard::BannerPixmap, QPixmap());

  setStyleSheet(R"(
    QWizard {
      background-color: #1f1f1f;
      color: #ffffff;
    }

    QWizardPage {
      background-color: #1f1f1f;
      color: #ffffff;
    }

    QLabel {
      color: #ffffff;
    }

    QWizard QLabel {
      color: #ffffff;
    }

    QWidget#qt_wizard_radiant {
      background-color: #1f1f1f;
    }

    QWidget#qt_wizard_pageframe {
      background-color: #1f1f1f;
    }

    QWidget#qt_wizard_header {
      background-color: #1f1f1f;
    }

    QPushButton {
      background-color: #2f2f2f;
      color: #ffffff;
      border: 1px solid #5a5a5a;
      border-radius: 4px;
      padding: 6px 12px;
    }

    QWizardPage > QLabel {
      background: transparent;
      color: #ffffff;
    }

    QPushButton:hover {
      background-color: #3a3a3a;
    }

    QPushButton:pressed {
      background-color: #262626;
    }
  )");

  state_.workingDir = workingDir;

  loadInitialState();

  addPage(new IntroPage(this));
  addPage(new IdentityPage(this));
  addPage(new CommitPage(this));
  addPage(new RemotePage(this));
  addPage(new PushPage(this));

  setField("userName", state_.userName);
  setField("userEmail", state_.userEmail);
  setField("useGlobalIdentity", state_.useGlobalIdentity);
  setField("createInitialCommit", state_.createInitialCommit);
  setField("commitMessage", state_.commitMessage);
  setField("connectRemote", state_.connectRemote);
  setField("remoteName", state_.remoteName);
  setField("remoteUrl", state_.remoteUrl);
  setField("pushAfterSetup", state_.pushAfterSetup);

  connect(this, &QWizard::accepted, this, [this]() {
    collectStateFromFields();
  });
}

NewProjectGitWizard::State NewProjectGitWizard::state() const {
  return state_;
}

void NewProjectGitWizard::loadInitialState() {
  if (!gitService_ || state_.workingDir.trimmed().isEmpty()) {
    return;
  }

  QString output;
  QString branchName;
  if (gitService_->currentBranch(state_.workingDir, &branchName, &output)) {
    state_.branchName = branchName;
  }

  QString userName;
  if (gitService_->getConfigValue(state_.workingDir, "user.name", false, &userName, &output)
      && !userName.trimmed().isEmpty()) {
    state_.userName = userName.trimmed();
  }

  QString userEmail;
  if (gitService_->getConfigValue(state_.workingDir, "user.email", false, &userEmail, &output)
      && !userEmail.trimmed().isEmpty()) {
    state_.userEmail = userEmail.trimmed();
  }
}

void NewProjectGitWizard::collectStateFromFields() {
  state_.userName = field("userName").toString().trimmed();
  state_.userEmail = field("userEmail").toString().trimmed();
  state_.useGlobalIdentity = field("useGlobalIdentity").toBool();
  state_.createInitialCommit = field("createInitialCommit").toBool();
  state_.commitMessage = field("commitMessage").toString().trimmed();
  state_.connectRemote = field("connectRemote").toBool();
  state_.remoteName = field("remoteName").toString().trimmed();
  state_.remoteUrl = field("remoteUrl").toString().trimmed();
  state_.pushAfterSetup = field("pushAfterSetup").toBool();
}