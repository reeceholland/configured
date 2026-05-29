#pragma once

#include <QString>

class ConfiguredProject;

/**
 * @brief Writes project parameters to a JSON export document.
 */
class JsonProjectExporter {
 public:
  /**
   * @brief Export collected project parameters to JSON.
   * @param project Source project.
   * @param filePath Destination JSON file path.
   * @param errorMessage Optional error output populated on failure.
   * @return true when the export file was written.
   */
  bool exportParameters(const ConfiguredProject& project, const QString& filePath,
                        QString* errorMessage = nullptr) const;
};
