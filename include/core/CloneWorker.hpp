#pragma once

#include <QObject>
#include <QProcess>
#include <QString>

class CloneWorker : public QObject {
  Q_OBJECT
 public:
  explicit CloneWorker(const QString& remoteUrl, const QString& parentFolder);

 public slots:
  void start();
  void cancel();

 signals:
  void outputReceived(const QString output);
  void finished(bool success, const QString& clonedPath, const QString& output);

 private:
  QString remoteUrl_;
  QString parentFolder_;
  QProcess* process_ = nullptr;

  QString deriveClonedPath() const;
};