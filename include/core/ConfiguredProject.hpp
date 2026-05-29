#pragma once

#include <QSet>
#include <QString>
#include <memory>
#include <vector>

#include "core/ConfiguredItem.hpp"
#include "export/ExportParameter.hpp"

class QJsonObject;

/**
 * @brief Owns the persisted project model and configuration item tree.
 *
 * ConfiguredProject stores project metadata, the root ConfiguredItem hierarchy,
 * JSON persistence, duplicate parameter checks, dirty-state clearing, and
 * exporter-friendly parameter collection.
 */
class ConfiguredProject {
 public:
  /// Creates an empty project with a root system item.
  ConfiguredProject();

  /// @name Project metadata
  /// @{
  const QString& name() const;
  void setName(const QString& name);

  const QString& description() const;
  void setDescription(const QString& description);

  const QString& author() const;
  void setAuthor(const QString& author);

  const QString& company() const;
  void setCompany(const QString& company);

  const QString& version() const;
  void setVersion(const QString& version);

  const QString& lastModified() const;
  void setLastModified(const QString& lastModified);

  const QString& robotPlatform() const;
  void setRobotPlatform(const QString& platform);

  bool isGitManaged() const;
  void setGitManaged(bool managed);
  /// @}

  /// Returns the mutable root item of the configuration hierarchy.
  ConfiguredItem* root();

  /// Returns the read-only root item of the configuration hierarchy.
  const ConfiguredItem* root() const;

  /// Replaces the current tree with a representative sample robotics project.
  void createSampleProject();

  /**
   * @brief Save the project as a .configured JSON document.
   * @param filePath Destination file path.
   * @param error Optional error output populated on failure.
   * @return true when the file was written successfully.
   */
  bool saveToFile(const QString& filePath, QString* error = nullptr);

  /**
   * @brief Load project metadata and items from a .configured JSON document.
   * @param filePath Source file path.
   * @param error Optional error output populated on failure.
   * @return true when the file was read and parsed successfully.
   */
  bool loadFromFile(const QString& filePath, QString* error = nullptr);

  /**
   * @brief Check whether two or more parameters share the same key.
   * @param duplicateKey Optional output containing the first duplicate key found.
   * @return true if at least one duplicate parameter key exists.
   */
  bool hasDuplicateParameterKeys(QString* duplicateKey = nullptr) const;

  const QString& gitCommitHash() const;
  void setGitCommitHash(const QString& hash);

  /**
   * @brief Flatten the item tree into exportable parameters.
   * @return Parameters with hierarchy path, type, key, value, and unit.
   */
  std::vector<ExportParameter> collectParameters() const;

  /// Clear dirty visual state flags across the full item tree.
  void clearDirtyFlags();

 private:
  QString name_;
  QString description_;
  QString author_;
  QString company_;
  QString version_;
  QString last_modified_;
  QString robot_platform_;
  bool git_managed_ = false;
  QString gitCommitHash_;
  std::unique_ptr<ConfiguredItem> root_;

  QJsonObject itemToJson(const ConfiguredItem* item) const;
  std::unique_ptr<ConfiguredItem> itemFromJson(const QJsonObject& obj);

  void collectParametersRecursive(const ConfiguredItem* item, const QString& parentPath,
                                  std::vector<ExportParameter>& out) const;
  void clearDirtyFlagsRecursive(ConfiguredItem* item);
};
