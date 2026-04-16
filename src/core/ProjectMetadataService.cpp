#include "core/ProjectMetadataService.hpp"

#include "core/ConfiguredProject.hpp"

bool ProjectMetadataService::validate(const ProjectMetadata& metadata, QString& error) {
  const QString name = metadata.name.trimmed();

  if (name.isEmpty()) {
    error = "Project name cannot be empty.";
    return false;
  }

  if (name.contains('/') || name.contains('\\')) {
    error = "Project name contains invalid characters.";
    return false;
  }

  return true;
}

void ProjectMetadataService::apply(ConfiguredProject& project, const ProjectMetadata& metadata) {
  project.setName(metadata.name);
  project.setDescription(metadata.description);
  project.setAuthor(metadata.author);
  project.setCompany(metadata.company);
  project.setVersion(metadata.version);
  project.setRobotPlatform(metadata.robotPlatform);
  project.setGitManaged(metadata.gitManaged);
}
