/**
 * @file ValidationSeverity.hpp
 * @author Reece Holland
 * @brief Represents the severity levels of validation messages in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

/**
 * @brief Represents the severity levels of validation messages in the Configured application.
 *
 * This enum defines the different severity levels that a validation message can have, such as
 * Warning and Error. It is used to categorize validation messages and determine their impact
 * on the overall validity of the configuration.
 *
 */
enum class ValidationSeverity { Warning, Error };