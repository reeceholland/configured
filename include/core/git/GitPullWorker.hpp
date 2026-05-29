#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

/**
 * @brief Runs git pull asynchronously and streams process output.
 */
class GitPullWorker : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Create a pull worker for a repository.
   * @param workingDir Repository working directory.
   */
  explicit GitPullWorker(const QString& workingDir);

 public slots:
  /// Start the pull process.
  void start();

  /// Request cancellation of the running pull process.
  void cancel();

 signals:
  /// Emitted whenever git writes output to stdout or stderr.
  void outputReceived(const QString output);

  /**
   * @brief Emitted once the pull process exits.
   * @param success true when git pull completed successfully.
   * @param output Full captured process output.
   */
  void finished(bool success, const QString& output);

 private:
  QString workingDir_;
  QProcess* process_ = nullptr;
};
