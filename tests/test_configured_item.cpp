#include <gtest/gtest.h>
#include "core/ConfiguredItem.hpp"

TEST(ConfiguredItemTest, StoresNameAndType)
{
    ConfiguredItem item("Drivetrain", ConfiguredItemType::Subsystem);

    EXPECT_EQ(item.name(), "Drivetrain");
    EXPECT_EQ(item.type(), ConfiguredItemType::Subsystem);
}

TEST(ConfiguredItemTest, AddsChildAndSetsParent)
{
    ConfiguredItem parent("Robot", ConfiguredItemType::Robot);
    auto child = std::make_unique<ConfiguredItem>("IMU", ConfiguredItemType::Component);
    ConfiguredItem *childPtr = child.get();

    parent.addChild(std::move(child));

    ASSERT_EQ(parent.children().size(), 1u);
    EXPECT_EQ(parent.children()[0].get(), childPtr);
    EXPECT_EQ(childPtr->parent(), &parent);
}