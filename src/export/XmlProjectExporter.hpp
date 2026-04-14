#pragma once

#include <QString>

class ConfiguredProject;

class XmlProjectExporter
{
public:
  bool exportParameters(const ConfiguredProject &project,
                        const QString &filePath,
                        QString *errorMessage = nullptr) const;
};