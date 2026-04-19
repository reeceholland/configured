#include <gtest/gtest.h>

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
