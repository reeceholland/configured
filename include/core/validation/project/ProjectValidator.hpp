#pragma once

#include "core/validation/ValidationResult.hpp"

class ConfiguredItem;
class ConfiguredProject;

class ProjectValidator {
 public:
  ValidationResult validate(const ConfiguredProject& project) const;

 private:
  void validateItemRecursive(const ConfiguredProject& project, const ConfiguredItem* item,
                             ValidationResult& result) const;
};