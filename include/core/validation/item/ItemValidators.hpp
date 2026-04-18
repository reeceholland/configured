/**
 * @file ItemValidators.hpp
 * @author Reece Holland
 * @brief Validators for item validation in the Configured application.
 * @version 0.1
 * @date 2026-04-18
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include "core/validation/IValidator.hpp"
#include "core/validation/item/ItemValidationContext.hpp"

/**
 * @brief Validator for required parameter keys in the Configured application.
 *
 * This validator checks that if an item is marked as required and is a parameter,
 * it must have a non-empty parameter key.
 *
 */
class RequiredParameterKeyValidator : public IValidator<ItemValidationContext> {
 public:
  /**
   * @brief Validates the required parameter key for the given context.
   *
   * @param context The context containing the item to be validated.
   * @param result The result object to store validation errors.
   */
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

/**
 * @brief Validator for required parameter values in the Configured application.
 *
 * This validator checks that if an item is marked as required and is a parameter,
 * it must have a non-empty parameter value.
 *
 */
class RequiredParameterValueValidator : public IValidator<ItemValidationContext> {
 public:
  /**
   * @brief Validates the required parameter value for the given context.
   *
   * @param context The context containing the item to be validated.
   * @param result The result object to store validation errors.
   */
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

/**
 * @brief Validator for duplicate parameter keys in the Configured application.
 *
 * This validator checks that there are no duplicate parameter keys within the project.
 * It traverses the project hierarchy to count occurrences of the same parameter key.
 *
 */
class DuplicateParameterKeyValidator : public IValidator<ItemValidationContext> {
 public:
  /**
   * @brief Validates duplicate parameter keys for the given context.
   *
   * @param context The context containing the item and project to be validated.
   * @param result The result object to store validation errors.
   */
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

/**
 * @brief Validator for invalid characters in item names in the Configured application.
 *
 * This validator checks that item names do not contain invalid characters such as slashes or
 * backslashes, which could cause issues with file paths or other processing.
 *
 */
class InvalidItemNameCharactersValidator : public IValidator<ItemValidationContext> {
 public:
  /**
   * @brief Validates item names for invalid characters in the given context.
   *
   * @param context The context containing the item to be validated.
   * @param result The result object to store validation errors.
   */
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};