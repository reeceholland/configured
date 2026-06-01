#pragma once

#include <QString>

/**
 * @brief Flattened parameter record used by project exporters.
 */
struct ExportParameter {
  /// Hierarchical path to the parameter within the project tree.
  QString path;

  /// Configured item/value type associated with the parameter.
  QString type;

  /// Stored parameter value as text.
  QString value;

  /// Optional engineering unit.
  QString unit;

  /// Unique parameter key used by downstream systems.
  QString key;
};
