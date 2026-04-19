/**
 * @file ItemValidator.hpp
 * @author Reece Holland
 * @brief Validator for configuration items in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include "core/validation/ValidationResult.hpp"
#include "core/validation/item/ItemValidationContext.hpp"

/**
 * @brief Validator for configuration items in the Configured application.
 *
 * This class provides a method to validate a configuration item based on various criteria,
 * such as required fields, duplicate keys, and invalid characters. It uses specific
 * validators for each validation rule and aggregates their results into a single
 * ValidationResult object.
 *
 */
class ItemValidator {
 public:
  /**
   * @brief Validates a configuration item based on the provided context.
   *
   * This method runs multiple validation checks on the item, including required parameter keys,
   * required parameter values, duplicate parameter keys, and invalid characters in item names.
   * The results of all validations are aggregated into a single ValidationResult object.
   *
   * @param context The context containing the item and project information for validation.
   * @return A ValidationResult object containing any validation errors or warnings.
   */
  ValidationResult validate(const ItemValidationContext& context) const;
};