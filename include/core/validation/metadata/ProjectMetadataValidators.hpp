/**
 * @file ProjectMetadataValidators.hpp
 * @author Reece Holland
 * @brief Validators for project metadata in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include "core/ProjectMetadata.hpp"
#include "core/validation/IValidator.hpp"

/**
 * @brief Validator for filesystem-safe project metadata names in the Configured application.
 *
 * This validator checks that the project metadata name does not contain any characters
 * that are invalid for filesystem paths, ensuring compatibility with file operations.
 */
class ProjectMetadataNameFilesystemSafeValidator : public IValidator<ProjectMetadata> {
 public:
  /**
   * @brief Validates the project metadata name for filesystem safety.
   *
   * This method checks if the project metadata name contains any characters
   * that are invalid for filesystem paths and updates the validation result accordingly.
   *
   * @param metadata The project metadata to be validated.
   * @param result The validation result to be updated with any errors or warnings.
   */
  void validate(const ProjectMetadata& metadata, ValidationResult& result) const override;
};

/**
 * @brief Validator for non-empty project metadata names in the Configured application.
 *
 * This validator checks that the project metadata name is not empty, ensuring that
 * every project has a valid name.
 */
class ProjectMetadataNameNotEmptyValidator : public IValidator<ProjectMetadata> {
 public:
  /**
   * @brief Validates the project metadata name for non-emptiness.
   *
   * This method checks if the project metadata name is empty and updates the validation result
   * accordingly.
   *
   * @param metadata The project metadata to be validated.
   * @param result The validation result to be updated with any errors or warnings.
   */
  void validate(const ProjectMetadata& metadata, ValidationResult& result) const override;
};