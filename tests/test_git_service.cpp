#include <gtest/gtest.h>

#include <QDir>
#include <QTemporaryDir>

#include "core/git/GitService.hpp"

TEST(GitServiceTest, InitRepositoryCreatesGitFolder) {
  GitService gitService;
  QTemporaryDir tempDir;

  QString output;
  const bool initResult = gitService.initRepository(tempDir.path(), &output);

  EXPECT_TRUE(initResult) << "Git repository should be initialized successfully. Output: "
                          << output.toStdString();

  QDir gitDir(tempDir.filePath(".git"));
  EXPECT_TRUE(gitDir.exists()) << ".git folder should exist after initializing repository.";
}

TEST(GitServiceTest, IsRepositoryDetectsGitRepo) {
  GitService gitService;
  QTemporaryDir tempDir;

  QString output;
  EXPECT_FALSE(gitService.isRepository(tempDir.path(), &output))
      << "Directory should not be recognized as a Git repository. Output: " << output.toStdString();

  EXPECT_TRUE(gitService.initRepository(tempDir.path(), &output))
      << "Git repository should be initialized successfully. Output: " << output.toStdString();

  EXPECT_TRUE(gitService.isRepository(tempDir.path(), &output))
      << "Directory should be recognized as a Git repository after initialization. Output: "
      << output.toStdString();
}

TEST(GitServiceTest, IsGitAvailableDetectsGit) {
  GitService gitService;

  QString output;
  const bool available = gitService.isGitAvailable(&output);

  EXPECT_TRUE(available) << "Git should be available on the system. Output: "
                         << output.toStdString();
}

TEST(GitServiceTest, WorkingTreeCleanOnNewRepo) {
  GitService gitService;
  QTemporaryDir tempDir;

  QString output;
  EXPECT_TRUE(gitService.initRepository(tempDir.path(), &output))
      << "Git repository should be initialized successfully. Output: " << output.toStdString();

  bool clean = false;
  EXPECT_TRUE(gitService.workingTreeClean(tempDir.path(), &clean, &output))
      << "Should be able to check if working tree is clean. Output: " << output.toStdString();
  EXPECT_TRUE(clean) << "Working tree should be clean immediately after initializing repository.";
}

TEST(GitServiceTest, WorkingTreeCleanReturnsFalseAfterEdit) {
  GitService gitService;
  QTemporaryDir tempDir;

  QString output;
  EXPECT_TRUE(gitService.initRepository(tempDir.path(), &output))
      << "Git repository should be initialized successfully. Output: " << output.toStdString();

  // Create a new file in the repository
  const QString newFilePath = tempDir.filePath("new_file.txt");
  QFile newFile(newFilePath);
  EXPECT_TRUE(newFile.open(QIODevice::WriteOnly)) << "Should be able to create new file.";
  newFile.write("Test content");
  newFile.close();

  bool clean = true;
  EXPECT_TRUE(gitService.workingTreeClean(tempDir.path(), &clean, &output))
      << "Should be able to check if working tree is clean. Output: " << output.toStdString();
  EXPECT_FALSE(clean) << "Working tree should not be clean after creating a new untracked file.";
}

TEST(GitServiceTest, ConnectRemoteAddsOrigin) {
  GitService gitService;
  QTemporaryDir tempDir;

  ASSERT_TRUE(tempDir.isValid()) << "Temporary directory should be created successfully.";

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output))
      << "Git repository should be initialized successfully. Output: " << output.toStdString();

  const QString remoteUrl = "https://github.com/reeceholland/configured_sample_project.git";

  ASSERT_TRUE(gitService.connectRemote(tempDir.path(), "origin", remoteUrl, &output))
      << "Should be able to connect remote. Output: " << output.toStdString();

  QString actualRemoteUrl;
  ASSERT_TRUE(gitService.remoteUrl(tempDir.path(), "origin", &actualRemoteUrl, &output))
      << "Should be able to get remote URL. Output: " << output.toStdString();

  EXPECT_EQ(actualRemoteUrl, remoteUrl) << "Remote URL should match the one that was set.";
}