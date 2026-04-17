#pragma once

#include <QString>

#include "core/validation/ValidationSeverity.hpp"

struct ValidationMessage {
  ValidationSeverity severity;
  QString code;
  QString message;
  QString field;
};