#include <gtest/gtest.h>

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredProject.hpp"

TEST(ValidationTest, DetectsDuplicateParameterKeys) {
  ConfiguredProject project;
  auto* root = project.root();

  auto subsystem = std::make_unique<ConfiguredItem>("Subsystem", ConfiguredItemType::Subsystem);
  auto component = std::make_unique<ConfiguredItem>("Component", ConfiguredItemType::Component);

  auto p1 = std::make_unique<ConfiguredItem>("Param1", ConfiguredItemType::Parameter);
  p1->setParameterKey("max_rpm");

  auto p2 = std::make_unique<ConfiguredItem>("Param2", ConfiguredItemType::Parameter);
  p2->setParameterKey("max_rpm");

  component->addChild(std::move(p1));
  component->addChild(std::move(p2));
  subsystem->addChild(std::move(component));
  root->addChild(std::move(subsystem));

  QString duplicate;
  EXPECT_TRUE(project.hasDuplicateParameterKeys(&duplicate));
  EXPECT_EQ(duplicate, "max_rpm");
}