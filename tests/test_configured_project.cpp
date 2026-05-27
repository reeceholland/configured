#include <gtest/gtest.h>

#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

#include "core/ConfiguredProject.hpp"

TEST(ConfiguredProjectTest, SaveAndLoadRoundTrip) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  const QString filePath = dir.filePath("test.configured");

  ConfiguredProject project;
  project.createSampleProject();
  project.setName("Test Project");
  project.setAuthor("Reece");
  project.setCompany("Example Co");
  project.setVersion("1.2.3");
  project.setRobotPlatform("Rover");

  QString saveError;
  ASSERT_TRUE(project.saveToFile(filePath, &saveError)) << saveError.toStdString();

  ConfiguredProject loaded;

  QString loadError;
  ASSERT_TRUE(loaded.loadFromFile(filePath, &loadError)) << loadError.toStdString();

  EXPECT_EQ(loaded.name(), "Test Project");
  EXPECT_EQ(loaded.author(), "Reece");
  EXPECT_EQ(loaded.company(), "Example Co");
  EXPECT_EQ(loaded.version(), "1.2.3");
  EXPECT_EQ(loaded.robotPlatform(), "Rover");
  ASSERT_NE(loaded.root(), nullptr);
}

TEST(ConfiguredProjectTest, LoadRejectsMalformedJson) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  const QString filePath = dir.filePath("broken.configured");
  QFile file(filePath);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("{ not valid json");
  file.close();

  ConfiguredProject project;
  QString error;

  EXPECT_FALSE(project.loadFromFile(filePath, &error));
  EXPECT_EQ(error, "Invalid JSON format.");
}

TEST(ConfiguredProjectTest, CollectParametersIncludesNestedPathAndFields) {
  ConfiguredProject project;
  project.createSampleProject();

  auto subsystem = std::make_unique<ConfiguredItem>("Drive", ConfiguredItemType::Subsystem);
  auto component = std::make_unique<ConfiguredItem>("LeftMotor", ConfiguredItemType::Component);
  auto parameter = std::make_unique<ConfiguredItem>("MaxRpm", ConfiguredItemType::Parameter);

  parameter->setParameterKey("max_rpm");
  parameter->setParameterValue("150");
  parameter->setParameterUnit("rpm");

  component->addChild(std::move(parameter));
  subsystem->addChild(std::move(component));
  project.root()->addChild(std::move(subsystem));

  const auto params = project.collectParameters();

  ASSERT_EQ(params.size(), 1u);
  EXPECT_EQ(params[0].path, "System.Drive.LeftMotor.MaxRpm");
  EXPECT_EQ(params[0].key, "max_rpm");
  EXPECT_EQ(params[0].value, "150");
  EXPECT_EQ(params[0].unit, "rpm");
  EXPECT_EQ(params[0].type, "string");
}
