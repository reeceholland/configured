#include "core/validation/item/ItemValidators.hpp"

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredProject.hpp"

namespace {
int countParameterKeyOccurrences(const ConfiguredItem* item, const QString& key) {
  if (!item) {
    return 0;
  }

  int count = 0;

  if (item->isParameter() && item->parameterKey().trimmed() == key) {
    ++count;
  }

  for (const auto& child : item->children()) {
    count += countParameterKeyOccurrences(child.get(), key);
  }

  return count;
}
}  // namespace

void RequiredParameterKeyValidator::validate(const ItemValidationContext& context,
                                             ValidationResult& result) const {
  if (!context.item || !context.item->isParameter()) {
    return;
  }

  if (context.item->required() && context.item->parameterKey().trimmed().isEmpty()) {
    result.addError("item.parameter.key.required", "Required parameters must have a key.",
                    "parameterKey");
  }
}

void RequiredParameterValueValidator::validate(const ItemValidationContext& context,
                                               ValidationResult& result) const {
  if (!context.item || !context.item->isParameter()) {
    return;
  }

  if (context.item->required() && context.item->parameterValue().trimmed().isEmpty()) {
    result.addError("item.parameter.value.required", "Required parameters must have a value.",
                    "parameterValue");
  }
}

void DuplicateParameterKeyValidator::validate(const ItemValidationContext& context,
                                              ValidationResult& result) const {
  if (!context.item || !context.project || !context.project->root()) {
    return;
  }

  if (!context.item->isParameter()) {
    return;
  }

  const QString key = context.item->parameterKey().trimmed();
  if (key.isEmpty()) {
    return;
  }

  if (countParameterKeyOccurrences(context.project->root(), key) > 1) {
    result.addError("item.parameter.key.duplicate",
                    "This parameter key is duplicated in the project.", "parameterKey");
  }
}

void FullstopItemNameValidator::validate(const ItemValidationContext& context,
                                         ValidationResult& result) const {
  if (!context.item) {
    return;
  }

  if (context.item->name().contains('.')) {
    result.addError("item.name.invalid_chars", "Item name cannot contain full stops.", "name");
  }
}