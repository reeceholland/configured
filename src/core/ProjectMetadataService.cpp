#include "core/ProjectMetadataService.hpp"

#include "core/ConfiguredProject.hpp"

void ProjectMetadataService::apply(ConfiguredProject& project, const ProjectMetadata& metadata) {
  project.setName(metadata.name);
  project.setDescription(metadata.description);
  project.setAuthor(metadata.author);
  project.setCompany(metadata.company);
  project.setVersion(metadata.version);
  project.setRobotPlatform(metadata.robotPlatform);
  project.setGitManaged(metadata.gitManaged);
}
