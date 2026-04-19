#include "core/validation/project/ProjectValidator.hpp"

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredProject.hpp"
#include "core/validation/item/ItemValidationContext.hpp"
#include "core/validation/item/ItemValidator.hpp"

ValidationResult ProjectValidator::validate(const ConfiguredProject& project) const {
  ValidationResult result;

  if (!project.root()) {
    result.addError("project.root.missing", "Project has no root item.", "root");
    return result;
  }

  validateItemRecursive(project, project.root(), result);
  return result;
}

void ProjectValidator::validateItemRecursive(const ConfiguredProject& project,
                                             const ConfiguredItem* item,
                                             ValidationResult& result) const {
  if (!item) {
    return;
  }

  ItemValidationContext context;
  context.project = &project;
  context.item = item;

  const ValidationResult itemResult = ItemValidator().validate(context);
  for (const ValidationMessage& message : itemResult.messages()) {
    if (message.severity == ValidationSeverity::Error) {
      result.addError(message.code, message.message, message.field);
    } else {
      result.addWarning(message.code, message.message, message.field);
    }
  }

  for (const auto& child : item->children()) {
    validateItemRecursive(project, child.get(), result);
  }
}
