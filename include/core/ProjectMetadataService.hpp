#pragma once

#include <QString>

#include "core/ProjectMetadata.hpp"

class ConfiguredProject;

class ProjectMetadataService {
 public:
  static void apply(ConfiguredProject& project, const ProjectMetadata& metadata);
};
