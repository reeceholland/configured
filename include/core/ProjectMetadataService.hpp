#pragma once

#include <QString>

#include "core/ProjectMetadata.hpp"

class ConfiguredProject;

/**
 * @brief Applies ProjectMetadata values to a ConfiguredProject.
 */
class ProjectMetadataService {
 public:
  /**
   * @brief Copy all metadata fields into the project model.
   * @param project Project to update.
   * @param metadata Values captured from the UI or project creation workflow.
   */
  static void apply(ConfiguredProject& project, const ProjectMetadata& metadata);
};
