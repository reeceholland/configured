/**
 * @file ProjectMetadataValidator.hpp
 * @author Reece Holland
 * @brief Validator for project metadata in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include "core/ProjectMetadata.hpp"
#include "core/validation/ValidationResult.hpp"

/**
 * @brief Validator for project metadata in the Configured application.
 *
 * This class provides a method to validate project metadata based on various criteria,
 * such as required fields and duplicate keys. It uses specific
 * validators for each validation rule and aggregates their results into a single
 * ValidationResult object.
 *
 */
class ProjectMetadataValidator {
 public:
  /**
   * @brief Validates project metadata based on the provided information.
   *
   * This method runs multiple validation checks on the project metadata and required fields.
   * The results of all validations are aggregated into a single ValidationResult object.
   *
   * @param metadata The project metadata to be validated.
   * @return A ValidationResult object containing any validation errors or warnings.
   */
  ValidationResult validate(const ProjectMetadata& metadata) const;
};