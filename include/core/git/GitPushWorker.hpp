#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

class GitPushWorker : public QObject {
  Q_OBJECT
 public:
  explicit GitPushWorker(const QString& workingDir, bool setUpstream, const QString& branchName);

 public slots:
  void start();
  void cancel();

 signals:
  void outputReceived(const QString output);
  void finished(bool success, const QString& output);

 private:
  QString workingDir_;
  QProcess* process_ = nullptr;
  bool setUpstream_;
  QString branchName_;
};