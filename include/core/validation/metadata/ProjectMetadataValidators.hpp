#pragma once

#include "core/ProjectMetadata.hpp"
#include "core/validation/IValidator.hpp"

class ProjectMetadataNameFilesystemSafeValidator : public IValidator<ProjectMetadata> {
 public:
  void validate(const ProjectMetadata& metadata, ValidationResult& result) const override;
};

class ProjectMetadataNameNotEmptyValidator : public IValidator<ProjectMetadata> {
 public:
  void validate(const ProjectMetadata& metadata, ValidationResult& result) const override;
};