#include "core/GitService.hpp"

#include <QProcess>

bool GitService::runGit(const QString& workingDir, const QStringList& arguments,
                        QString* output) const {
  QProcess process;

  if (!workingDir.trimmed().isEmpty()) {
    process.setWorkingDirectory(workingDir);
  }

  process.start("git", arguments);

  if (!process.waitForStarted()) {
    if (output) {
      *output = "Failed to start git.";
    }
    return false;
  }

  if (!process.waitForFinished()) {
    if (output) {
      *output = "Git command timed out.";
    }
    return false;
  }

  const QString stdoutText = QString::fromUtf8(process.readAllStandardOutput());
  const QString stderrText = QString::fromUtf8(process.readAllStandardError());
  const QString combined = (stdoutText + "\n" + stderrText).trimmed();

  if (output) {
    *output = combined;
  }

  return process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

bool GitService::isGitAvailable(QString* output) const {
  return runGit(QString(), {"--version"}, output);
}

bool GitService::isRepository(const QString& workingDir, QString* output) const {
  return runGit(workingDir, {"rev-parse", "--is-inside-work-tree"}, output);
}

bool GitService::initRepository(const QString& workingDir, QString* output) const {
  return runGit(workingDir, {"init"}, output);
}

bool GitService::status(const QString& workingDir, QString* output) const {
  return runGit(workingDir, {"status", "--short"}, output);
}

bool GitService::addAll(const QString& workingDir, QString* output) const {
  return runGit(workingDir, {"add", "."}, output);
}

bool GitService::commit(const QString& workingDir, const QString& message, QString* output) const {
  return runGit(workingDir, {"commit", "-m", message}, output);
}

bool GitService::getConfigValue(const QString& workingDir, const QString& key, bool global,
                                QString* value, QString* output) const {
  QStringList args = {"config"};
  if (global) {
    args << "--global";
  }
  args << "--get" << key;

  QString result;
  const bool ok = runGit(global ? QString() : workingDir, args, &result);

  if (output) {
    *output = result;
  }

  if (value) {
    *value = result.trimmed();
  }

  return ok;
}

bool GitService::setConfigValue(const QString& workingDir, const QString& key, const QString& value,
                                bool global, QString* output) const {
  QStringList args = {"config"};
  if (global) {
    args << "--global";
  }
  args << key << value;

  return runGit(global ? QString() : workingDir, args, output);
}

bool GitService::getCommitHash(const QString& workingDir, QString* hash, QString* output) const {
  QString result;

  const bool ok = runGit(workingDir, {"rev-parse", "--short", "HEAD"}, &result);

  if (output) {
    *output = result;
  }

  if (hash) {
    *hash = result.trimmed();
  }

  return ok;
}