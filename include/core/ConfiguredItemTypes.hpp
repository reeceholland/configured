#pragma once

#include <QString>

#include "core/ConfiguredItem.hpp"

QString configuredItemTypeToString(ConfiguredItemType type);
ConfiguredItemType configuredItemTypeFromString(const QString& text);