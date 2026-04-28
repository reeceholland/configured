#pragma once

#include <QObject>
#include <QString>

class QWidget;
class ConfiguredProject;
class GitService;
class ProjectService;

class GitWorkflowController : public QObject {
  Q_OBJECT

 public:
  struct ProjectContext {
    ConfiguredProject* project = nullptr;
    QString projectFilePath;
    bool* hasUnsavedChanges = nullptr;
  };

  explicit GitWorkflowController(GitService* gitService, ProjectService* projectService,
                                 QWidget* dialogParent = nullptr, QObject* parent = nullptr);

  void setDialogParent(QWidget* dialogParent);
  void setProjectContext(const ProjectContext& context);

  void showGitStatus();
  void promptAndCommit();
  void promptAndPull();
  void promptAndPush();
  void promptAndConnectRemote();
  void promptAndSwitchBranch();
  void runNewProjectGitOnboarding();

 signals:
  void reloadProjectRequested(const QString& projectFilePath);
  void gitStateChanged();
  void informationRequested(const QString& title, const QString& message);
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
