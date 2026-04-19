/**
 * @file ItemValidationContext.hpp
 * @brief Context for item validation in the Configured application.
 * @version 0.1
 * @date 2026-04-19
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */
#pragma once

class ConfiguredItem;
class ConfiguredProject;

/**
 * @brief Context for item validation in the Configured application.
 *
 * This struct provides the necessary context for validating a configuration item,
 * including a pointer to the item being validated and a pointer to the overall project.
 *
 */
struct ItemValidationContext {
  /// The item being validated. This may be null if the validation is not specific to an item.
  const ConfiguredItem* item = nullptr;

  /// The project containing the item. This may be null if the validation is not specific to a
  /// project.
  const ConfiguredProject* project = nullptr;
};