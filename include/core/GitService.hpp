#pragma once

#include <QString>
#include <QStringList>

class GitService {
 public:
  bool isGitAvailable(QString* output = nullptr) const;
  bool isRepository(const QString& workingDir, QString* output = nullptr) const;
  bool initRepository(const QString& workingDir, QString* output = nullptr) const;
  bool status(const QString& workingDir, QString* output = nullptr) const;
  bool addAll(const QString& workingDir, QString* output = nullptr) const;
  bool commit(const QString& workingDir, const QString& message, QString* output = nullptr) const;
  bool getConfigValue(const QString& workingDir, const QString& key, bool global, QString* value,
                      QString* output = nullptr) const;

  bool setConfigValue(const QString& workingDir, const QString& key, const QString& value,
                      bool global, QString* output = nullptr) const;

  bool getCommitHash(const QString& workingDir, QString* hash, QString* output = nullptr) const;

  bool cloneRepository(const QString& remoteUrl, const QString& parentFolder, QString* clonedPath,
                       QString* output = nullptr) const;

 private:
  bool runGit(const QString& workingDir, const QStringList& arguments,
              QString* output = nullptr) const;
};