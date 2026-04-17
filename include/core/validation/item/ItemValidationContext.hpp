#pragma once

class ConfiguredItem;
class ConfiguredProject;

struct ItemValidationContext {
  const ConfiguredItem* item = nullptr;
  const ConfiguredProject* project = nullptr;
};