#pragma once

#include <QString>

class ConfiguredProject;

class RosYamlExporter {
 public:
  bool exportParameters(const ConfiguredProject& project, const QString& filePath,
                        QString* errorMessage = nullptr) const;
};