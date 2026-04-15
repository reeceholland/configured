/**
 * @file ConfiguredItem.hpp
 * @author Reece Holland
 * @brief Represents a configurable item in the Configured application.
 * @version 0.3
 * @date 2026-04-15
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QString>
#include <memory>
#include <vector>

/**
 * @brief Represents the type of a configurable item.
 *
 */
enum class ConfiguredItemType { System, Subsystem, Component, Parameter };

/**
 * @brief Represents the type of a parameter value.
 *
 */
enum class ConfiguredValueType { None, String, Integer, Double, Boolean };

/**
 * @brief Represents a configurable item in the Configured application.
 *
 * A ConfiguredItem can represent a robot, subsystem, component, or parameter.
 * It can have a name, description, and type. If it is a parameter, it can also
 * have a key, value, unit, and value type. Each ConfiguredItem can have child
 * items, allowing for a hierarchical structure of robots, subsystems, components,
 * and parameters.
 *
 */
class ConfiguredItem {
 public:
  /**
   * @brief Constructs a ConfiguredItem with the given name and type.
   *
   * @param name The name of the item.
   * @param type The type of the item (default is Component).
   */
  explicit ConfiguredItem(const QString& name,
                          ConfiguredItemType type = ConfiguredItemType::Component);

  /**
   * @brief Destroy the Configured Item object
   *
   */
  ~ConfiguredItem() = default;

  /**
   * @brief Returns the name of the item.
   */
  const QString& name() const;

  /**
   * @brief Sets the name of the item.
   */
  void setName(const QString& name);

  /**
   * @brief Returns the type of the item.
   */
  ConfiguredItemType type() const;

  /**
   * @brief Sets the type of the item.
   */
  void setType(ConfiguredItemType type);

  /**
   * @brief Returns the description of the item.
   */
  const QString& description() const;

  /**
   * @brief Sets the description of the item.
   */
  void setDescription(const QString& description);

  /**
   * @brief Returns the key of the parameter.
   */
  const QString& parameterKey() const;

  /**
   * @brief Sets the key of the parameter.
   */
  void setParameterKey(const QString& key);

  /**
   * @brief Returns the value of the parameter.
   */
  const QString& parameterValue() const;

  /**
   * @brief Sets the value of the parameter.
   */
  void setParameterValue(const QString& value);

  /**
   * @brief Returns the unit of the parameter.
   */
  const QString& parameterUnit() const;

  /**
   * @brief Sets the unit of the parameter.
   */
  void setParameterUnit(const QString& unit);

  /**
   * @brief Returns true if the parameter is required.
   */
  bool required() const;
  /**
   * @brief Sets whether the parameter is required.
   */
  void setRequired(bool required);

  /**
   * @brief Returns the parent of the item.
   */
  ConfiguredItem* parent();

  /**
   * @brief Returns the parent of the item (const version).
   */
  const ConfiguredItem* parent() const;

  /**
   * @brief Adds a child item to this item.
   *
   * @param child The child item to add.
   */
  void addChild(std::unique_ptr<ConfiguredItem> child);

  /**
   * @brief Removes a child item from this item.
   *
   * @param childToRemove The child item to remove.
   * @return true if the child was found and removed, false otherwise.
   */
  bool removeChild(ConfiguredItem* childToRemove);

  /**
   * @brief Returns the list of child items.
   */
  std::vector<std::unique_ptr<ConfiguredItem>>& children();

  /**
   * @brief Returns the list of child items (const version).
   */
  const std::vector<std::unique_ptr<ConfiguredItem>>& children() const;

  /**
   * @brief Returns true if this item should be exported as a parameter.
   */
  bool isParameter() const;

  /**
   * @brief Returns the value type as a string for export.
   *
   */
  QString valueTypeString() const;

  /**
   * @brief Returns the parameter value as a string for export.
   */
  QString valueAsString() const;

  /**
   * @brief Returns the parameter key as a string for export.
   */
  QString keyAsString() const;

  /**
   * @brief Returns the parameter unit as a string for export.
   */
  QString unitAsString() const;

 private:
  /// @brief The name of the item.
  QString name_;

  /// @brief The type of the item (System, Subsystem, Component, Parameter).
  ConfiguredItemType type_;

  /// @brief The description of the item.
  QString description_;

  /// @brief The key of the parameter (if this item is a parameter).
  QString parameterKey_;

  /// @brief The value of the parameter (if this item is a parameter).
  QString parameterValue_;

  /// @brief The unit of the parameter (if this item is a parameter).
  QString parameterUnit_;

  /// @brief Whether the parameter is required (if this item is a parameter).
  bool required_ = false;

  /// @brief The type of the parameter value (if this item is a parameter).
  ConfiguredValueType valueType_ = ConfiguredValueType::None;

  QString stringValue_;
  int intValue_ = 0;
  double doubleValue_ = 0.0;
  bool boolValue_ = false;

  ConfiguredItem* parent_ = nullptr;
  std::vector<std::unique_ptr<ConfiguredItem>> children_;
};