#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

class GitPullWorker : public QObject {
  Q_OBJECT
 public:
  explicit GitPullWorker(const QString& workingDir);

 public slots:
  void start();
  void cancel();

 signals:
  void outputReceived(const QString output);
  void finished(bool success, const QString& output);

 private:
  QString workingDir_;
  QProcess* process_ = nullptr;
};