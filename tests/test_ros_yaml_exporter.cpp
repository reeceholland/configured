#include <gtest/gtest.h>

#include <QFile>
#include <QTemporaryDir>
#include <memory>
#include <yaml-cpp/yaml.h>

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredProject.hpp"
#include "export/RosYamlExporter.hpp"

namespace {

std::unique_ptr<ConfiguredItem> makeParameter(const QString& key, const QString& value) {
  auto parameter = std::make_unique<ConfiguredItem>(key, ConfiguredItemType::Parameter);
  parameter->setParameterKey(key);
  parameter->setParameterValue(value);
  return parameter;
}

}  // namespace

TEST(RosYamlExporterTest, WritesRosNodeParameterYaml) {
  QTemporaryDir dir;
  ASSERT_TRUE(dir.isValid());

  ConfiguredProject project;
  project.setName("slam_toolbox");
  project.root()->addChild(makeParameter("use_sim_time", "true"));
  project.root()->addChild(makeParameter("mode", "mapping"));
  project.root()->addChild(makeParameter("scan_topic", "/scan"));
  project.root()->addChild(makeParameter("max_laser_range", "12.0"));

  const QString filePath = dir.filePath("slam_toolbox.yaml");

  RosYamlExporter exporter;
  QString error;
  ASSERT_TRUE(exporter.exportParameters(project, filePath, &error)) << error.toStdString();

  const YAML::Node root = YAML::LoadFile(filePath.toStdString());
  const YAML::Node params = root["slam_toolbox"]["ros__parameters"];

  ASSERT_TRUE(params);
  EXPECT_TRUE(params["use_sim_time"].as<bool>());
  EXPECT_EQ(params["mode"].as<std::string>(), "mapping");
  EXPECT_EQ(params["scan_topic"].as<std::string>(), "/scan");
  EXPECT_DOUBLE_EQ(params["max_laser_range"].as<double>(), 12.0);
}
