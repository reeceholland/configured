#pragma once

#include <QString>

class ConfiguredProject;

class JsonProjectExporter {
 public:
  bool exportParameters(const ConfiguredProject& project, const QString& filePath,
                        QString* errorMessage = nullptr) const;
};