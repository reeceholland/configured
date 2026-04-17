#include "core/validation/metadata/ProjectMetadataValidators.hpp"

void ProjectMetadataNameFilesystemSafeValidator::validate(const ProjectMetadata& metadata,
                                                          ValidationResult& result) const {
  const QString name = metadata.name.trimmed();

  if (name.contains('/') || name.contains('\\')) {
    result.addError("metadata.name.invalid_chars", "Project name contains invalid path characters.",
                    "name");
  }
}

void ProjectMetadataNameNotEmptyValidator::validate(const ProjectMetadata& metadata,
                                                    ValidationResult& result) const {
  if (metadata.name.trimmed().isEmpty()) {
    result.addError("ProjectMetadata.NameEmpty", "Project name cannot be empty.", "name");
  }
}
