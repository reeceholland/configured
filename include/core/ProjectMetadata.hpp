#pragma once

#include <QString>

struct ProjectMetadata {
  QString name;
  QString description;
  QString author;
  QString company;
  QString version;
  QString robotPlatform;
  bool gitManaged = false;

  QString lastModified;
  QString gitCommitHash;
};