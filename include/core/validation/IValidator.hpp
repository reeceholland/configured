#pragma once

#include "core/validation/ValidationResult.hpp"

template <typename T>
class IValidator {
 public:
  virtual ~IValidator() = default;
  virtual void validate(const T& target, ValidationResult& result) const = 0;
};