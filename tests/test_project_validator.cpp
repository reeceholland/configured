/**
 * @file test_project_validator.cpp
 * @brief Unit tests for whole-project validation rules.
 *
 * These tests verify that ProjectValidator walks the project hierarchy and
 * applies item validation rules across nested configuration items.
 */

#include <gtest/gtest.h>

#include "core/ConfiguredProject.hpp"
#include "core/validation/project/ProjectValidator.hpp"

TEST(ProjectValidatorTest, RejectsRequiredParameterWithoutValue) {
  ConfiguredProject project;
  project.createSampleProject();

  auto param = std::make_unique<ConfiguredItem>("RequiredParam", ConfiguredItemType::Parameter);
  param->setRequired(true);
  param->setParameterKey("speed");

  project.root()->addChild(std::move(param));

  ProjectValidator validator;
  const ValidationResult result = validator.validate(project);

  EXPECT_FALSE(result.isValid());
}

TEST(ProjectValidatorTest, AcceptsValidNestedProject) {
  ConfiguredProject project;
  project.createSampleProject();

  auto subsystem = std::make_unique<ConfiguredItem>("Drive", ConfiguredItemType::Subsystem);
  auto component = std::make_unique<ConfiguredItem>("Motor", ConfiguredItemType::Component);
  auto param = std::make_unique<ConfiguredItem>("MaxSpeed", ConfiguredItemType::Parameter);

  param->setRequired(true);
  param->setParameterKey("max_speed");
  param->setParameterValue("10");

  component->addChild(std::move(param));
  subsystem->addChild(std::move(component));
  project.root()->addChild(std::move(subsystem));

  ProjectValidator validator;
  const ValidationResult result = validator.validate(project);

  EXPECT_TRUE(result.isValid());
}