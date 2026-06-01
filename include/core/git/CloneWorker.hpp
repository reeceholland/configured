#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

/**
 * @brief Runs git clone asynchronously and streams process output.
 *
 * CloneWorker is designed to live on a worker thread. Call start() from that
 * thread and listen for finished() to move back to the UI workflow.
 */
class CloneWorker : public QObject {
  Q_OBJECT
 public:
  /**
   * @brief Create a clone worker.
   * @param remoteUrl Repository URL to clone.
   * @param parentFolder Destination parent folder.
   */
  explicit CloneWorker(const QString& remoteUrl, const QString& parentFolder);

 public slots:
  /// Start the clone process.
  void start();

  /// Request cancellation of the running clone process.
  void cancel();

 signals:
  /// Emitted whenever git writes output to stdout or stderr.
  void outputReceived(const QString output);

  /**
   * @brief Emitted once the clone process exits.
   * @param success true when git clone completed successfully.
   * @param clonedPath Best-effort path to the cloned repository.
   * @param output Full captured process output.
   */
  void finished(bool success, const QString& clonedPath, const QString& output);

 private:
  QString remoteUrl_;
  QString parentFolder_;
  QString output_;
  QProcess* process_ = nullptr;

  void appendProcessOutput(const QString& text);
  QString deriveClonedPath() const;
};
