#pragma once

#include <QDialog>
#include <QString>

/**
 * @brief Dialog shown before pulling when local work may need attention.
 */
class PullPreflightDialog : public QDialog {
  Q_OBJECT
 public:
  /**
   * @brief User decision selected from the preflight dialog.
   */
  enum class Decision { SaveProject, CommitChanges, Cancel };

  /**
   * @brief Repository and editor state shown in the preflight dialog.
   */
  struct State {
    /// True when the editor has changes that are not saved to the project file.
    bool hasUnsavedEditorChanges = false;

    /// True when the Git working tree has uncommitted changes.
    bool hasUncommittedGitChanges = false;

    /// True when local commits have not been pushed upstream.
    bool hasUnpushedCommits = false;

    /// Current local branch name.
    QString branchName;

    /// Upstream branch name, if one is configured.
    QString upstreamName;
  };

  /// Create the dialog for the current pull preflight state.
  explicit PullPreflightDialog(const State& state, QWidget* parent = nullptr);

  /// Return the decision selected by the user.
  Decision decision() const;

 private:
  Decision decision_ = Decision::Cancel;
};
