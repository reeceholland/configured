#include "core/ConfiguredItemTypes.hpp"

#include <string>

QString configuredItemTypeToString(ConfiguredItemType type) {
  switch (type) {
    case ConfiguredItemType::System:
      return "System";
    case ConfiguredItemType::Subsystem:
      return "Subsystem";
    case ConfiguredItemType::Component:
      return "Component";
    case ConfiguredItemType::Parameter:
      return "Parameter";
    default:
      return "Unknown";
  }
}

ConfiguredItemType configuredItemTypeFromString(const QString& text) {
  if (text == "System") {
    return ConfiguredItemType::System;
  }
  if (text == "Subsystem") {
    return ConfiguredItemType::Subsystem;
  }
  if (text == "Component") {
    return ConfiguredItemType::Component;
  }
  if (text == "Parameter") {
    return ConfiguredItemType::Parameter;
  }
  return ConfiguredItemType::Component;  // Default to Component if unknown
}