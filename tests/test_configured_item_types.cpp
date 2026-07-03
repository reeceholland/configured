#include "core/ConfiguredItemTypes.hpp"
#include "gtest/gtest.h"

TEST(ConfiguredItemTypeTest, ConvertsItemTypeToDisplayString) {
  EXPECT_EQ(configuredItemTypeToString(ConfiguredItemType::System), "System");
  EXPECT_EQ(configuredItemTypeToString(ConfiguredItemType::Subsystem), "Subsystem");
  EXPECT_EQ(configuredItemTypeToString(ConfiguredItemType::Component), "Component");
  EXPECT_EQ(configuredItemTypeToString(ConfiguredItemType::Parameter), "Parameter");
}

TEST(ConfiguredItemTypeTest, ConvertsDisplayStringToItemType) {
  EXPECT_EQ(configuredItemTypeFromString("System"), ConfiguredItemType::System);
  EXPECT_EQ(configuredItemTypeFromString("Subsystem"), ConfiguredItemType::Subsystem);
  EXPECT_EQ(configuredItemTypeFromString("Component"), ConfiguredItemType::Component);
  EXPECT_EQ(configuredItemTypeFromString("Parameter"), ConfiguredItemType::Parameter);

  // Test unknown string returns default type (Component)
  EXPECT_EQ(configuredItemTypeFromString("Unknown"), ConfiguredItemType::Component);
}