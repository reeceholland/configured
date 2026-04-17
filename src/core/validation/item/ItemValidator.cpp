#include "core/validation/item/ItemValidator.hpp"

#include "core/validation/item/ItemValidators.hpp"

ValidationResult ItemValidator::validate(const ItemValidationContext& context) const {
  ValidationResult result;

  RequiredParameterKeyValidator().validate(context, result);
  RequiredParameterValueValidator().validate(context, result);
  DuplicateParameterKeyValidator().validate(context, result);
  FullstopItemNameValidator().validate(context, result);

  return result;
}