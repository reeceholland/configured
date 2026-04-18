/**
 * @file ValidationResult.hpp
 * @author Reece Holland
 * @brief Represents the result of a validation process in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QList>
#include <QString>

#include "core/validation/ValidationMessage.hpp"

/**
 * @brief Represents the result of a validation operation.
 *
 * This class collects validation messages (errors and warnings) generated
 * during validation. It provides helper methods to query the overall validity
 * and access all generated messages.
 */
class ValidationResult {
 public:
  /**
   * @brief Adds an error message to the validation result.
   *
   * @param code A machine-readable error code.
   * @param message A human-readable description of the error.
   * @param field (Optional) The field associated with the error.
   */
  void addError(const QString& code, const QString& message, const QString& field = "");

  /**
   * @brief Adds a warning message to the validation result.
   *
   * @param code A machine-readable warning code.
   * @param message A human-readable description of the warning.
   * @param field (Optional) The field associated with the warning.
   */
  void addWarning(const QString& code, const QString& message, const QString& field = "");

  /**
   * @brief Checks if the validation result is valid.
   *
   * A validation result is considered valid if it contains no errors.
   *
   * @return true if there are no errors, false otherwise.
   */
  bool isValid() const;

  /**
   * @brief Checks if the validation result contains any errors.
   *
   * @return true if one or more errors exist, false otherwise.
   */
  bool hasErrors() const;

  /**
   * @brief Checks if the validation result contains any warnings.
   *
   * @return true if one or more warnings exist, false otherwise.
   */
  bool hasWarnings() const;

  /**
   * @brief Returns all validation messages.
   *
   * This includes both errors and warnings.
   *
   * @return A list of validation messages.
   */
  const QList<ValidationMessage>& messages() const;

 private:
  /**
   * @brief Internal storage of validation messages.
   */
  QList<ValidationMessage> messages_;
};