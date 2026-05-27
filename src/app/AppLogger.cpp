#include "app/AppLogger.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QTextStream>
#include <QtCore/QtLogging>

namespace {
QMutex logMutex;
QString currentLogFilePath;

constexpr qint64 kMaxLogFileBytes = 1024 * 1024;
constexpr int kMaxRotatedLogFiles = 3;

QString messageTypeName(QtMsgType type) {
  switch (type) {
    case QtDebugMsg:
      return "DEBUG";
    case QtInfoMsg:
      return "INFO";
    case QtWarningMsg:
      return "WARN";
    case QtCriticalMsg:
      return "ERROR";
    case QtFatalMsg:
      return "FATAL";
  }

  return "UNKNOWN";
}

QString defaultLogDirectory() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
  if (path.trimmed().isEmpty()) {
    path = QDir::temp().filePath("Configured");
  }

  return QDir(path).filePath("logs");
}

void rotateLogFileIfNeeded(const QString& filePath) {
  const QFileInfo info(filePath);
  if (!info.exists() || info.size() < kMaxLogFileBytes) {
    return;
  }

  QFile::remove(filePath + "." + QString::number(kMaxRotatedLogFiles));

  for (int i = kMaxRotatedLogFiles - 1; i >= 1; --i) {
    const QString from = filePath + "." + QString::number(i);
    const QString to = filePath + "." + QString::number(i + 1);

    if (QFileInfo::exists(from)) {
      QFile::rename(from, to);
    }
  }

  QFile::rename(filePath, filePath + ".1");
}

void writeLogMessage(QtMsgType type, const QMessageLogContext& context, const QString& message) {
  QMutexLocker locker(&logMutex);

  if (currentLogFilePath.trimmed().isEmpty()) {
    return;
  }

  rotateLogFileIfNeeded(currentLogFilePath);

  QFile file(currentLogFilePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
    return;
  }

  QTextStream stream(&file);
  stream << QDateTime::currentDateTime().toString(Qt::ISODateWithMs) << " ["
         << messageTypeName(type) << "] " << qFormatLogMessage(type, context, message)
         << Qt::endl;
}
}  // namespace

bool AppLogger::install(QString* error) {
  const QString logDirectory = defaultLogDirectory();
  QDir dir;

  if (!dir.mkpath(logDirectory)) {
    if (error) {
      *error = "Could not create log directory: " + logDirectory;
    }
    return false;
  }

  currentLogFilePath = QDir(logDirectory).filePath("configured.log");

  qSetMessagePattern(
      "%{if-category}%{category}: %{endif}%{message}"
      "%{if-debug} (%{file}:%{line}, %{function})%{endif}");
  qInstallMessageHandler(writeLogMessage);

  qInfo() << "Logging initialized at" << currentLogFilePath;
  return true;
}

QString AppLogger::logFilePath() {
  QMutexLocker locker(&logMutex);
  return currentLogFilePath;
}
