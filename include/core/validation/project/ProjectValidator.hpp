#pragma once

#include "core/validation/ValidationResult.hpp"

class ConfiguredItem;
class ConfiguredProject;

/**
 * @brief Runs whole-project validation across metadata and item hierarchy.
 */
class ProjectValidator {
 public:
  /**
   * @brief Validate a project and all configuration items.
   * @param project Project to validate.
   * @return Validation messages collected from project and item validators.
   */
  ValidationResult validate(const ConfiguredProject& project) const;

 private:
  void validateItemRecursive(const ConfiguredProject& project, const ConfiguredItem* item,
                             ValidationResult& result) const;
};
