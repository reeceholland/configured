#pragma once

#include <QDialog>
#include <QString>

class PullPreflightDialog : public QDialog {
  Q_OBJECT
 public:
  enum class Decision { SaveProject, CommitChanges, Cancel };

  struct State {
    bool hasUnsavedEditorChanges = false;
    bool hasUncommittedGitChanges = false;
    bool hasUnpushedCommits = false;
    QString branchName;
    QString upstreamName;
  };

  explicit PullPreflightDialog(const State& state, QWidget* parent = nullptr);

  Decision decision() const;

 private:
  Decision decision_ = Decision::Cancel;
};