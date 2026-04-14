#pragma once

#include <QString>
#include <vector>
#include <memory>

enum class ConfiguredItemType
{
  Robot,
  Subsystem,
  Component,
  Parameter
};

enum class ConfiguredValueType
{
  None,
  String,
  Integer,
  Double,
  Boolean
};

class ConfiguredItem
{
public:
  explicit ConfiguredItem(const QString &name,
                          ConfiguredItemType type = ConfiguredItemType::Component);
  ~ConfiguredItem() = default;

  const QString &name() const;
  void setName(const QString &name);

  ConfiguredItemType type() const;
  void setType(ConfiguredItemType type);

  const QString &description() const;
  void setDescription(const QString &description);

  const QString &parameterKey() const;
  void setParameterKey(const QString &key);

  const QString &parameterValue() const;
  void setParameterValue(const QString &value);

  const QString &parameterUnit() const;
  void setParameterUnit(const QString &unit);

  bool required() const;
  void setRequired(bool required);

  ConfiguredItem *parent();
  const ConfiguredItem *parent() const;

  void addChild(std::unique_ptr<ConfiguredItem> child);
  bool removeChild(ConfiguredItem *childToRemove);

  std::vector<std::unique_ptr<ConfiguredItem>> &children();
  const std::vector<std::unique_ptr<ConfiguredItem>> &children() const;

  bool isParameter() const;

  /**
   * @brief Returns true if this item should be exported as a parameter.
   */
  bool isParameter() const;

  /**
   * @brief Returns the parameter type as a string for export.
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
  QString name_;
  ConfiguredItemType type_;
  QString description_;

  QString parameterKey_;
  QString parameterValue_;
  QString parameterUnit_;
  bool required_ = false;

  ConfiguredValueType valueType_ = ConfiguredValueType::None;

  QString stringValue_;
  int intValue_ = 0;
  double doubleValue_ = 0.0;
  bool boolValue_ = false;

  ConfiguredItem *parent_ = nullptr;
  std::vector<std::unique_ptr<ConfiguredItem>> children_;
};