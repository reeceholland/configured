#include "ui/PullPreflightDialog.hpp"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

PullPreflightDialog::PullPreflightDialog(const State& state, QWidget* parent) : QDialog(parent) {
  setWindowTitle("Pull Remote Changes");
  setModal(true);
  resize(460, 220);

  auto* rootLayout = new QVBoxLayout(this);

  auto* titleLabel =
      new QLabel("Pulling remote changes may overwrite or conflict with your local changes");
  titleLabel->setWordWrap(true);
  titleLabel->setStyleSheet("font-weight: 600;");

  auto* detailLabel = new QLabel("Choose how to handle local changes before continuing.", this);
  detailLabel->setWordWrap(true);

  auto* statusLayout = new QFormLayout();
  statusLayout->setLabelAlignment(Qt::AlignLeft);
  statusLayout->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);

  auto yesNoText = [](bool value) {
    return value ? "Yes" : "No";
  };

  auto* unsavedLabel = new QLabel(yesNoText(state.hasUnsavedEditorChanges), this);
  auto* uncommittedLabel = new QLabel(yesNoText(state.hasUncommittedGitChanges), this);
  auto* unpushedLabel = new QLabel(yesNoText(state.hasUnpushedCommits), this);
  auto* branchLabel =
      new QLabel(state.branchName.isEmpty() ? "Not available" : state.branchName, this);
  auto* upstreamLabel =
      new QLabel(state.upstreamName.isEmpty() ? "Not configured" : state.upstreamName, this);

  statusLayout->addRow("Unsaved editor changes:", unsavedLabel);
  statusLayout->addRow("Uncommitted Git changes:", uncommittedLabel);
  statusLayout->addRow("Unpushed commits:", unpushedLabel);
  statusLayout->addRow("Branch:", branchLabel);
  statusLayout->addRow("Upstream:", upstreamLabel);

  auto* buttonRow = new QHBoxLayout();

  auto* saveButton = new QPushButton("Save Project", this);
  auto* commitButton = new QPushButton("Commit Changes", this);
  auto* cancelButton = new QPushButton("Cancel", this);

  buttonRow->addWidget(saveButton);
  buttonRow->addWidget(commitButton);
  buttonRow->addStretch();
  buttonRow->addWidget(cancelButton);

  rootLayout->addWidget(titleLabel);
  rootLayout->addWidget(detailLabel);
  rootLayout->addLayout(statusLayout);
  rootLayout->addSpacing(8);
  rootLayout->addLayout(buttonRow);

  connect(saveButton, &QPushButton::clicked, this, [this]() {
    decision_ = Decision::SaveProject;
    accept();
  });

  connect(commitButton, &QPushButton::clicked, this, [this]() {
    decision_ = Decision::CommitChanges;
    accept();
  });

  connect(cancelButton, &QPushButton::clicked, this, [this]() {
    decision_ = Decision::Cancel;
    reject();
  });
}

PullPreflightDialog::Decision PullPreflightDialog::decision() const {
  return decision_;
}