#pragma once

#include <QObject>
#include <QString>

class QLabel;
class QStatusBar;
class QWidget;
class ConfiguredProject;
class GitService;

/**
 * @brief Keeps the editor status bar synchronized with project and Git state.
 */
class StatusBarController : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Current editor/project state used to decide what should be shown.
   */
  struct Context {
    /// Active project, or nullptr when no project is loaded.
    ConfiguredProject* project = nullptr;

    /// Path to the active .configured file.
    QString projectFilePath;

    /// True when the editor has unsaved changes.
    bool hasUnsavedChanges = false;

    /// True when the editor screen is currently visible.
    bool editorVisible = false;
  };

  /**
   * @brief Widgets owned by MainWindow and updated by this controller.
   */
  struct Widgets {
    /// Status bar that contains the labels and refresh control.
    QStatusBar* statusBar = nullptr;

    /// Label showing the configured remote URL.
    QLabel* remoteUrlLabel = nullptr;

    /// Label showing the current Git branch.
    QLabel* branchLabel = nullptr;

    /// Label showing whether the project has unsaved editor changes.
    QLabel* dirtyLabel = nullptr;

    /// Label showing whether commits are ahead of the upstream branch.
    QLabel* commitLabel = nullptr;

    /// Refresh control shown when editor status is available.
    QWidget* refreshButton = nullptr;
  };

  /**
   * @brief Create a status bar controller.
   * @param gitService Service used to query repository state.
   * @param parent QObject parent.
   */
  explicit StatusBarController(GitService* gitService, QObject* parent = nullptr);

  /// Replace the state used by the next refresh.
  void setContext(const Context& context);

  /// Register the status bar widgets to update.
  void setWidgets(const Widgets& widgets);

  /// Refresh all status bar fields from project and repository state.
  void refresh();

  /// Clear status text and hide repository-only controls.
  void clear();

 private:
  QString workingDir() const;
  bool shouldShowStatus() const;
  bool isRepository(QString* output = nullptr) const;

  void updateRemoteUrl();
  void updateBranch();
  void updateDirtyState();
  void updateCommitState();

  GitService* gitService_ = nullptr;
  Context context_;
  Widgets widgets_;
};
