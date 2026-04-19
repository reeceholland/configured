#include "core/validation/ValidationResult.hpp"

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
  for (const auto& message : messages_) {
    if (message.severity == ValidationSeverity::Error) {
      return true;
    }
  }
  return false;
}

bool ValidationResult::hasWarnings() const {
  for (const auto& message : messages_) {
    if (message.severity == ValidationSeverity::Warning) {
      return true;
    }
  }
  return false;
}

const QList<ValidationMessage>& ValidationResult::messages() const {
  return messages_;
}
