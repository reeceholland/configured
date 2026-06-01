#pragma once

#include <QString>

/**
 * @brief Editable project-level metadata shown in the new/open project dialogs.
 *
 * ProjectMetadata is a transfer object used by UI dialogs and services before
 * values are applied to a ConfiguredProject. Keeping it separate from the
 * project model makes validation and metadata updates explicit.
 */
struct ProjectMetadata {
  /// Human-readable project name. Also used to derive default project filenames.
  QString name;

  /// Optional project description shown in metadata dialogs and exported files.
  QString description;

  /// Optional author name for project ownership/audit metadata.
  QString author;

  /// Optional company or organization name.
  QString company;

  /// User-facing project version string.
  QString version;

  /// Robot, vehicle, or platform this configuration targets.
  QString robotPlatform;

  /// True when the project should be initialized and managed as a Git repository.
  bool gitManaged = false;

  /// Last modified timestamp stored in the project file.
  QString lastModified;

  /// Commit hash associated with the saved project state, when available.
  QString gitCommitHash;
};
