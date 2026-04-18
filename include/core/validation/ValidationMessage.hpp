/**
 * @file ValidationMessage.hpp
 * @author Reece Holland
 * @brief Represents a validation message in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QString>

#include "core/validation/ValidationSeverity.hpp"

/**
 * @brief Represents a validation message in the Configured application.
 *
 * This struct encapsulates information about a validation message, including its severity,
 * a unique code for identification, the human-readable message, and the specific field
 * associated with the validation issue. It is used to communicate validation results
 * throughout the application.
 *
 */
struct ValidationMessage {
  /// @brief The severity level of the validation message, indicating.
  ValidationSeverity severity;
  /// @brief A unique code identifying the specific validation message.
  QString code;
  /// @brief A human-readable message describing the validation issue.
  QString message;
  /// @brief The specific field associated with the validation issue, if applicable.
  QString field;
};