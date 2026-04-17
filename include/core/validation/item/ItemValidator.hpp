#pragma once

#include "core/validation/ValidationResult.hpp"
#include "core/validation/item/ItemValidationContext.hpp"

class ItemValidator {
 public:
  ValidationResult validate(const ItemValidationContext& context) const;
};