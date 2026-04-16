#pragma once

#include <QString>

#include "core/ProjectMetadata.hpp"

class ConfiguredProject;

class ProjectMetadataService {
 public:
  static bool validate(const ProjectMetadata& metadata, QString& error);

  static void apply(ConfiguredProject& project, const ProjectMetadata& metadata);
};
