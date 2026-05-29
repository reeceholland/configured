#pragma once

#include <QString>

class ConfiguredProject;

/**
 * @brief Writes project parameters to an XML export document.
 */
class XmlProjectExporter {
 public:
  /**
   * @brief Export collected project parameters to XML.
   * @param project Source project.
   * @param filePath Destination XML file path.
   * @param errorMessage Optional error output populated on failure.
   * @return true when the export file was written.
   */
  bool exportParameters(const ConfiguredProject& project, const QString& filePath,
                        QString* errorMessage = nullptr) const;
};
