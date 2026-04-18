/**
 * @file IValidator.hpp
 * @author Reece Holland
 * @brief Interface for validators in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include "core/validation/ValidationResult.hpp"

/**
 * @brief Interface for validators in the Configured application.
 *
 * This interface defines the contract for all validators, which are responsible for
 * validating specific types of objects and updating a ValidationResult with any
 * errors or warnings.
 *
 * @tparam T The type of object to be validated.
 */
template <typename T>
class IValidator {
 public:
  /**
   * @brief Destroy the IValidator object
   *
   */
  virtual ~IValidator() = default;

  /**
   * @brief Validates the given target object and updates the validation result.
   *
   * This method must be implemented by all concrete validators. It takes an object of type T
   * and a ValidationResult reference, performs validation checks on the object, and updates
   * the ValidationResult with any errors or warnings found during validation.
   *
   * @param target The object to be validated.
   * @param result The ValidationResult to be updated with any validation errors or warnings.
   */
  virtual void validate(const T& target, ValidationResult& result) const = 0;
};