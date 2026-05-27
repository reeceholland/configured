#include "core/git/CloneWorker.hpp"

#include <QDir>
#include <QProcess>

CloneWorker::CloneWorker(const QString& remoteUrl, const QString& parentFolder)
    : remoteUrl_(remoteUrl), parentFolder_(parentFolder) {}

void CloneWorker::start() {
  if (remoteUrl_.trimmed().isEmpty()) {
    emit finished(false, QString(), "Remote URL must be provided.");
    return;
  }

  if (!QDir(parentFolder_).exists()) {
    emit finished(false, QString(), "Parent folder does not exist: " + parentFolder_);
    return;
  }

  process_ = new QProcess(this);
  process_->setWorkingDirectory(parentFolder_);

  // Git usually writes progress information to stdout for some transports.
  connect(process_, &QProcess::readyReadStandardOutput, this, [this]() {
    const QString text = QString::fromUtf8(process_->readAllStandardOutput());
    appendProcessOutput(text);
  });

  // Clone progress and authentication errors commonly arrive on stderr, so treat it as
  // user-visible output rather than as an immediate failure condition.
  connect(process_, &QProcess::readyReadStandardError, this, [this]() {
    const QString text = QString::fromUtf8(process_->readAllStandardError());
    appendProcessOutput(text);
  });

  // The process is only considered successful if Git exits normally with code 0. A killed
  // process, crash, authentication failure, or invalid remote URL will report failure here.
  connect(process_, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
          [this](int exitCode, QProcess::ExitStatus exitStatus) {
            appendProcessOutput(QString::fromUtf8(process_->readAllStandardOutput()));
            appendProcessOutput(QString::fromUtf8(process_->readAllStandardError()));

            const bool success = exitStatus == QProcess::NormalExit && exitCode == 0;
            QString output = output_.trimmed();
            if (!success && output.isEmpty()) {
              output = QString("git clone failed with exit code %1.").arg(exitCode);
            }

            emit finished(success, success ? deriveClonedPath() : QString(), output);
          });

  // Use the system Git executable. The working directory controls where Git creates the
  // cloned repository folder.
  process_->start("git", {"clone", remoteUrl_});

  // waitForStarted() waits only for process startup, not for the clone to complete. The
  // long-running clone still completes asynchronously through the finished() signal above.
  if (!process_->waitForStarted()) {
    emit finished(false, QString(), "Failed to start git clone: " + process_->errorString());
  }
}

void CloneWorker::cancel() {
  if (!process_) {
    return;
  }

  process_->kill();
}

void CloneWorker::appendProcessOutput(const QString& text) {
  if (text.isEmpty()) {
    return;
  }

  output_ += text;
  emit outputReceived(text);
}

QString CloneWorker::deriveClonedPath() const {
  QString repoName = remoteUrl_.trimmed();

  while (repoName.endsWith('/')) {
    repoName.chop(1);
  }

  // HTTPS-style URLs end with "/repo.git"; SSH SCP-style URLs may look like
  // "git@host:owner/repo.git". Applying both section calls handles both common forms.
  repoName = repoName.section('/', -1);
  repoName = repoName.section(':', -1);

  if (repoName.endsWith(".git")) {
    repoName.chop(4);
  }

  return QDir(parentFolder_).filePath(repoName);
}
