#include "core/validation/metadata/ProjectMetadataValidator.hpp"

#include "core/validation/metadata/ProjectMetadataValidators.hpp"

ValidationResult ProjectMetadataValidator::validate(const ProjectMetadata& metadata) const {
  ValidationResult result;

  ProjectMetadataNameNotEmptyValidator().validate(metadata, result);
  ProjectMetadataNameFilesystemSafeValidator().validate(metadata, result);

  return result;
}