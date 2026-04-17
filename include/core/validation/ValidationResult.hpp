#pragma once

#include <QList>
#include <QString>

#include "core/validation/ValidationMessage.hpp"

class ValidationResult {
 public:
  void addError(const QString& code, const QString& message, const QString& field = "");
  void addWarning(const QString& code, const QString& message, const QString& field = "");

  bool isValid() const;
  bool hasErrors() const;
  bool hasWarnings() const;

  const QList<ValidationMessage>& messages() const;

 private:
  QList<ValidationMessage> messages_;
};