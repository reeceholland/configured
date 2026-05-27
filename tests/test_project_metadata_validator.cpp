#include <gtest/gtest.h>

#include "core/ProjectMetadata.hpp"
#include "core/validation/metadata/ProjectMetadataValidator.hpp"

TEST(ProjectMetadataValidatorTest, RejectsEmptyProjectName) {
  ProjectMetadata metadata;
  metadata.name = "   ";

  const ValidationResult result = ProjectMetadataValidator().validate(metadata);

  EXPECT_FALSE(result.isValid());
}

TEST(ProjectMetadataValidatorTest, RejectsPathSeparatorsInProjectName) {
  ProjectMetadata metadata;
  metadata.name = "robot/config";

  const ValidationResult result = ProjectMetadataValidator().validate(metadata);

  EXPECT_FALSE(result.isValid());
}

TEST(ProjectMetadataValidatorTest, AcceptsFilesystemSafeProjectName) {
  ProjectMetadata metadata;
  metadata.name = "Rover_Config_01";

  const ValidationResult result = ProjectMetadataValidator().validate(metadata);

  EXPECT_TRUE(result.isValid());
}
