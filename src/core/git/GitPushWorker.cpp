#include "core/git/GitPushWorker.hpp"

#include <QProcess>

GitPushWorker::GitPushWorker(const QString& workingDir, bool setUpstream, const QString& branchName)
    : workingDir_(workingDir), setUpstream_(setUpstream), branchName_(branchName) {}

void GitPushWorker::start() {
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

  if (setUpstream_) {
    process_->start("git", {"push", "-u", "origin", branchName_});
  } else {
    process_->start("git", {"push"});
  }

  if (!process_->waitForStarted()) {
    emit finished(false, "Failed to start git push.");
  }
}

void GitPushWorker::cancel() {
  if (!process_) {
    return;
  }

  process_->kill();
}
