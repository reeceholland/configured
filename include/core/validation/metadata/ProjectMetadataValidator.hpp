#pragma once

#include "core/ProjectMetadata.hpp"
#include "core/validation/ValidationResult.hpp"

class ProjectMetadataValidator {
 public:
  ValidationResult validate(const ProjectMetadata& metadata) const;
};