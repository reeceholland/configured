#pragma once

#include <QObject>
#include <QString>

class QWidget;
class ConfiguredProject;
class GitService;
class ProjectService;

/**
 * @brief Handles user-facing Git workflows for the active project.
 *
 * The controller keeps Git dialogs and application actions out of MainWindow.
 * It owns no project data; instead it receives a ProjectContext that points at
 * the currently loaded project and emits signals when the UI should refresh.
 */
class GitWorkflowController : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Runtime project state required to run Git actions.
   */
  struct ProjectContext {
    /// Active project model, or nullptr when no project is open.
    ConfiguredProject* project = nullptr;

    /// Path to the active .configured file.
    QString projectFilePath;

    /// Pointer to MainWindow's unsaved-change flag so save/pull decisions stay current.
    bool* hasUnsavedChanges = nullptr;
  };

  /**
   * @brief Create a Git workflow controller.
   * @param gitService Service used to execute Git commands.
   * @param projectService Service used to save/reload projects around Git actions.
   * @param dialogParent Parent widget for modal dialogs.
   * @param parent QObject parent.
   */
  explicit GitWorkflowController(GitService* gitService, ProjectService* projectService,
                                 QWidget* dialogParent = nullptr, QObject* parent = nullptr);

  /// Update the parent widget used for dialogs created by this controller.
  void setDialogParent(QWidget* dialogParent);

  /// Replace the active project context used by future Git actions.
  void setProjectContext(const ProjectContext& context);

  /// Show a read-only Git status dialog for the active repository.
  void showGitStatus();

  /// Prompt for a commit message and commit project/repository changes.
  void promptAndCommit();

  /// Run pull preflight checks, then pull remote changes if the user proceeds.
  void promptAndPull();

  /// Push the current branch, prompting for upstream setup when needed.
  void promptAndPush();

  /// Prompt for a remote URL and connect or clone a repository.
  void promptAndConnectRemote();

  /// Prompt for a target branch and switch the active repository.
  void promptAndSwitchBranch();

  /// Launch the first-run Git setup wizard for newly created projects.
  void runNewProjectGitOnboarding();

 signals:
  /// Emitted when Git changed files and the project should be reloaded from disk.
  void reloadProjectRequested(const QString& projectFilePath);

  /// Emitted after an action that may change branch, dirty state, or upstream state.
  void gitStateChanged();

  /// Request that the host UI show an informational message.
  void informationRequested(const QString& title, const QString& message);

  /// Request that the host UI show a warning message.
  void warningRequested(const QString& title, const QString& message);

 private:
  QString workingDir() const;
  bool ensureProjectAvailable(QString* errorMessage = nullptr) const;
  bool ensureRepository(QString* errorMessage = nullptr) const;

  GitService* gitService_ = nullptr;
  ProjectService* projectService_ = nullptr;
  QWidget* dialogParent_ = nullptr;
  ProjectContext context_;
};
