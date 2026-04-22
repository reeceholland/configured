#include "core/git/GitPullWorker.hpp"

#include <QProcess>

GitPullWorker::GitPullWorker(const QString& workingDir) : workingDir_(workingDir) {}

void GitPullWorker::start() {
  process_ = new QProcess(this);
  process_->setWorkingDirectory(workingDir_);

  connect(process_, &QProcess::readyReadStandardOutput, this, [this]() {
    const QString text = QString::fromUtf8(process_->readAllStandardOutput());
    emit outputReceived(text);
  });

  connect(process_, &QProcess::readyReadStandardError, this, [this]() {
    const QString text = QString::fromUtf8(process_->readAllStandardError());
    emit outputReceived(text);
  });

  connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          [this](int exitCode, QProcess::ExitStatus exitStatus) {
            const QString stdoutText = QString::fromUtf8(process_->readAllStandardOutput());
            const QString stderrText = QString::fromUtf8(process_->readAllStandardError());
            const QString output = (stdoutText + "\n" + stderrText).trimmed();

            const bool success = exitStatus == QProcess::NormalExit && exitCode == 0;

            emit finished(success, output);
          });

  process_->start("git", {"pull"});

  if (!process_->waitForStarted()) {
    emit finished(false, "Failed to start git pull.");
  }
}

void GitPullWorker::cancel() {
  if (!process_) {
    return;
  }

  process_->kill();
}
