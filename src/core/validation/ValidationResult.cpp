#include "core/validation/ValidationResult.hpp"

#include <algorithm>

void ValidationResult::addError(const QString& code, const QString& message, const QString& field) {
  messages_.append({ValidationSeverity::Error, code, message, field});
}

void ValidationResult::addWarning(const QString& code, const QString& message,
                                  const QString& field) {
  messages_.append({ValidationSeverity::Warning, code, message, field});
}

bool ValidationResult::isValid() const {
  return !hasErrors();
}

bool ValidationResult::hasErrors() const {
  return std::ranges::any_of(messages_, [](const ValidationMessage& message) {
    return message.severity == ValidationSeverity::Error;
  });
}

bool ValidationResult::hasWarnings() const {
  return std::ranges::any_of(messages_, [](const ValidationMessage& message) {
    return message.severity == ValidationSeverity::Warning;
  });
}

const QList<ValidationMessage>& ValidationResult::messages() const {
  return messages_;
}
