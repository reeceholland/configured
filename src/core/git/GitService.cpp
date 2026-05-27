#include "core/git/GitService.hpp"

#include <QDir>
#include <QtCore/QLoggingCategory>
#include <QProcess>

Q_LOGGING_CATEGORY(logGitService, "configured.core.git")

bool GitService::runGit(const QString& workingDir, const QStringList& arguments,
                        QString* output) const {
  QProcess process;

  if (!workingDir.trimmed().isEmpty()) {
    process.setWorkingDirectory(workingDir);
  }

  qCInfo(logGitService) << "Running git" << arguments.join(' ') << "in"
                        << (workingDir.trimmed().isEmpty() ? QString("<default>") : workingDir);

  process.start("git", arguments);

  if (!process.waitForStarted()) {
    if (output) {
      *output = "Failed to start git.";
    }
    qCWarning(logGitService) << "Failed to start git" << arguments.join(' ');
    return false;
  }

  if (!process.waitForFinished()) {
    if (output) {
      *output = "Git command timed out.";
    }
    qCWarning(logGitService) << "Git command timed out" << arguments.join(' ');
    return false;
  }

  const QString stdoutText = QString::fromUtf8(process.readAllStandardOutput());
  const QString stderrText = QString::fromUtf8(process.readAllStandardError());
  const QString combined = (stdoutText + "\n" + stderrText).trimmed();

  if (output) {
    *output = combined;
  }

  const bool success = process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
  if (!success) {
    qCWarning(logGitService) << "Git command failed" << arguments.join(' ') << combined;
  }

  return success;
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

bool GitService::cloneRepository(const QString& remoteUrl, const QString& parentFolder,
                                 QString* clonedPath, QString* output) const {
  if (remoteUrl.trimmed().isEmpty() || parentFolder.trimmed().isEmpty()) {
    if (output) {
      *output = "Remote URL and parent folder must be provided.";
    }
    return false;
  }

  const bool ok = runGit(parentFolder, {"clone", remoteUrl}, output);
  if (ok && clonedPath) {
    const QString repoName = remoteUrl.section('/', -1).remove(".git");
    *clonedPath = QDir(parentFolder).filePath(repoName);
  }

  return ok;
}

bool GitService::remoteExists(const QString& workingDir, const QString& name,
                              QString* output) const {
  return runGit(workingDir, {"remote", "get-url", name}, output);
}

bool GitService::addRemote(const QString& workingDir, const QString& name, const QString& url,
                           QString* output) const {
  return runGit(workingDir, {"remote", "add", name, url}, output);
}

bool GitService::setRemoteUrl(const QString& workingDir, const QString& name, const QString& url,
                              QString* output) const {
  return runGit(workingDir, {"remote", "set-url", name, url}, output);
}

bool GitService::connectRemote(const QString& workingDir, const QString& name, const QString& url,
                               QString* output) const {
  if (workingDir.trimmed().isEmpty()) {
    if (output) {
      *output = "Working directory is empty.";
    }
    return false;
  }

  if (name.trimmed().isEmpty()) {
    if (output) {
      *output = "Remote name is empty.";
    }
    return false;
  }

  if (url.trimmed().isEmpty()) {
    if (output) {
      *output = "Remote URL is empty.";
    }
    return false;
  }

  QString remoteOutput;
  if (remoteExists(workingDir, name.trimmed(), &remoteOutput)) {
    return setRemoteUrl(workingDir, name.trimmed(), url.trimmed(), output);
  }

  return addRemote(workingDir, name.trimmed(), url.trimmed(), output);
}

bool GitService::workingTreeClean(const QString& workingDir, bool* clean, QString* output) const {
  if (clean) {
    *clean = false;
  }

  QString statusOutput;
  if (!runGit(workingDir, {"status", "--porcelain"}, &statusOutput)) {
    if (output) {
      *output = statusOutput;
    }
    return false;
  }

  if (clean) {
    *clean = statusOutput.trimmed().isEmpty();
  }

  if (output) {
    *output = statusOutput;
  }
  return true;
}

bool GitService::remoteUrl(const QString& workingDir, const QString& name, QString* url,
                           QString* output) const {
  QString result;
  const bool ok = runGit(workingDir, {"remote", "get-url", name}, &result);

  if (output) {
    *output = result;
  }

  if (ok && url) {
    *url = result.trimmed();
  }

  return ok;
}

bool GitService::currentBranch(const QString& workingDir, QString* branchName,
                               QString* output) const {
  QString result;
  const bool ok = runGit(workingDir, {"branch", "--show-current"}, &result);

  if (output) {
    *output = result;
  }

  if (ok && branchName) {
    *branchName = result.trimmed();
  }

  return ok;
}

bool GitService::listLocalBranches(const QString& workingDir, QStringList* branches,
                                   QString* output) const {
  if (branches) {
    branches->clear();
  }

  QString result;
  const bool ok = runGit(workingDir, {"branch", "--format=%(refname:short)"}, &result);

  if (output) {
    *output = result;
  }

  if (!ok) {
    return false;
  }

  if (branches) {
    const QStringList lines = result.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
      const QString branch = line.trimmed();
      if (!branch.isEmpty()) {
        branches->append(branch);
      }
    }
  }

  return true;
}

bool GitService::listRemoteBranches(const QString& workingDir, QStringList* branches,
                                    QString* output) const {
  if (branches) {
    branches->clear();
  }

  QString result;
  const bool ok = runGit(workingDir, {"branch", "--remotes", "--format=%(refname:short)"}, &result);

  if (output) {
    *output = result;
  }

  if (!ok) {
    return false;
  }

  if (branches) {
    const QStringList lines = result.split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines) {
      const QString branch = line.trimmed();
      if (branch.isEmpty() || branch.endsWith("/HEAD")) {
        continue;
      }
      branches->append(branch);
    }
  }

  return true;
}

bool GitService::switchBranch(const QString& workingDir, const QString& branchName,
                              QString* output) const {
  const QString trimmedBranchName = branchName.trimmed();
  if (trimmedBranchName.isEmpty()) {
    if (output) {
      *output = "Branch name is empty.";
    }
    return false;
  }

  return runGit(workingDir, {"switch", trimmedBranchName}, output);
}

bool GitService::switchToRemoteBranch(const QString& workingDir, const QString& remoteBranchName,
                                      QString* output) const {
  const QString trimmedBranchName = remoteBranchName.trimmed();
  if (trimmedBranchName.isEmpty()) {
    if (output) {
      *output = "Remote branch name is empty.";
    }
    return false;
  }

  return runGit(workingDir, {"switch", "--track", trimmedBranchName}, output);
}

bool GitService::hasUnpushedCommits(const QString& workingDir, bool* hasUnpushedCommits,
                                    QString* output) const {
  if (!hasUnpushedCommits) {
    if (output) {
      *output = "hasUnpushedCommits pointer is null.";
    }
    return false;
  }

  *hasUnpushedCommits = false;

  if (workingDir.trimmed().isEmpty()) {
    if (output) {
      *output = "Working directory is empty.";
    }
    return false;
  }

  QString repoOutput;
  if (!isRepository(workingDir, &repoOutput)) {
    if (output) {
      *output = "Not a Git repository: " + repoOutput;
    }
    return false;
  }

  QString remoteOutput;
  if (!remoteExists(workingDir, "origin", &remoteOutput)) {
    if (output) {
      *output = "Remote 'origin' is not configured.";
    }
    return false;
  }

  QString result;
  const bool ok = runGit(workingDir, {"rev-list", "--count", "@{u}..HEAD"}, &result);

  if (output) {
    *output = result;
  }

  if (!ok) {
    return false;
  }

  bool parsed = false;
  const int unpushedCount = result.trimmed().toInt(&parsed);

  if (!parsed) {
    if (output) {
      *output = "Could not parse unpushed commit count: " + result;
    }
    return false;
  }

  *hasUnpushedCommits = unpushedCount > 0;
  return true;
}

bool GitService::hasUpstream(const QString& workingDir, bool* hasUpstream, QString* output) const {
  if (!hasUpstream) {
    if (output) {
      *output = "hasUpstream pointer is null.";
    }
    return false;
  }

  *hasUpstream = false;

  if (workingDir.trimmed().isEmpty()) {
    if (output) {
      *output = "Working directory is empty.";
    }
    return false;
  }

  QString repoOutput;
  if (!isRepository(workingDir, &repoOutput)) {
    if (output) {
      *output = "Not a Git repository: " + repoOutput;
    }
    return false;
  }

  QString result;
  const bool ok =
      runGit(workingDir, {"rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"}, &result);

  if (output) {
    *output = result;
  }

  *hasUpstream = ok;
  return true;
}

bool GitService::fetch(const QString& workingDir, QString* output) const {
  return runGit(workingDir, {"fetch"}, output);
}
