#pragma once

#include <QObject>
#include <QString>

class QLabel;
class QStatusBar;
class QWidget;
class ConfiguredProject;
class GitService;

class StatusBarController : public QObject {
  Q_OBJECT

 public:
  struct Context {
    ConfiguredProject* project = nullptr;
    QString projectFilePath;
    bool hasUnsavedChanges = false;
    bool editorVisible = false;
  };

  struct Widgets {
    QStatusBar* statusBar = nullptr;
    QLabel* remoteUrlLabel = nullptr;
    QLabel* branchLabel = nullptr;
    QLabel* dirtyLabel = nullptr;
    QLabel* commitLabel = nullptr;
    QWidget* refreshButton = nullptr;
  };

  explicit StatusBarController(GitService* gitService, QObject* parent = nullptr);

  void setContext(const Context& context);
  void setWidgets(const Widgets& widgets);

  void refresh();
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