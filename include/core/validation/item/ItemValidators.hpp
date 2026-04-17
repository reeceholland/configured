#pragma once

#include "core/validation/IValidator.hpp"
#include "core/validation/item/ItemValidationContext.hpp"

class RequiredParameterKeyValidator : public IValidator<ItemValidationContext> {
 public:
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

class RequiredParameterValueValidator : public IValidator<ItemValidationContext> {
 public:
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

class DuplicateParameterKeyValidator : public IValidator<ItemValidationContext> {
 public:
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};

class FullstopItemNameValidator : public IValidator<ItemValidationContext> {
 public:
  void validate(const ItemValidationContext& context, ValidationResult& result) const override;
};