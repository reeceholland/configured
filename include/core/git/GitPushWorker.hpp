#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

/**
 * @brief Runs git push asynchronously and streams process output.
 */
class GitPushWorker : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Create a push worker for a repository.
   * @param workingDir Repository working directory.
   * @param setUpstream true to push with upstream tracking for branchName.
   * @param branchName Current branch name used when setting upstream.
   */
  explicit GitPushWorker(const QString& workingDir, bool setUpstream, const QString& branchName);

 public slots:
  /// Start the push process.
  void start();

  /// Request cancellation of the running push process.
  void cancel();

 signals:
  /// Emitted whenever git writes output to stdout or stderr.
  void outputReceived(const QString output);

  /**
   * @brief Emitted once the push process exits.
   * @param success true when git push completed successfully.
   * @param output Full captured process output.
   */
  void finished(bool success, const QString& output);

 private:
  QString workingDir_;
  QProcess* process_ = nullptr;
  bool setUpstream_;
  QString branchName_;
};
