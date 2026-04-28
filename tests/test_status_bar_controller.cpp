#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QLabel>
#include <QProcess>
#include <QStatusBar>
#include <QTemporaryDir>
#include <QToolButton>

#include "app/StatusBarController.hpp"
#include "core/ConfiguredProject.hpp"
#include "core/git/GitService.hpp"

TEST(StatusBarControllerTest, RefreshHidesStatusBarWhenEditorIsNotVisible) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;

  controller.setContext(context);
  controller.refresh();

  EXPECT_FALSE(statusBar.isVisible());
  EXPECT_FALSE(refreshButton.isVisible());

  EXPECT_TRUE(remoteUrlLabel.text().isEmpty());
  EXPECT_TRUE(branchLabel.text().isEmpty());
  EXPECT_TRUE(dirtyLabel.text().isEmpty());
  EXPECT_TRUE(commitLabel.text().isEmpty());
}

TEST(StatusBarControllerTest, RefreshHidesStatusBarWhenProjectIsNotGitManaged) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(false);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;

  controller.setContext(context);
  controller.refresh();

  EXPECT_FALSE(statusBar.isVisible());
  EXPECT_FALSE(refreshButton.isVisible());

  EXPECT_TRUE(remoteUrlLabel.text().isEmpty());
  EXPECT_TRUE(branchLabel.text().isEmpty());
  EXPECT_TRUE(dirtyLabel.text().isEmpty());
  EXPECT_TRUE(commitLabel.text().isEmpty());
}

TEST(StatusBarControllerTest, RefreshClearsLabelsWhenStatusIsHidden) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  remoteUrlLabel.setText("Remote: origin");
  branchLabel.setText("Branch: main");
  dirtyLabel.setText("Project: Dirty");
  commitLabel.setText("Unpushed commits");

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};

  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = false;

  controller.setContext(context);
  controller.refresh();

  EXPECT_FALSE(statusBar.isVisible());
  EXPECT_FALSE(refreshButton.isVisible());

  EXPECT_TRUE(remoteUrlLabel.text().isEmpty());
  EXPECT_TRUE(branchLabel.text().isEmpty());
  EXPECT_TRUE(dirtyLabel.text().isEmpty());
  EXPECT_TRUE(commitLabel.text().isEmpty());
}

TEST(StatusBarControllerTest, RefreshShowsRemoteUrlWhenOriginExists) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();
  ASSERT_TRUE(
      gitService.addRemote(tempDir.path(), "origin", "https://example.com/repo.git", &output))
      << output.toStdString();

  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());

  EXPECT_EQ(remoteUrlLabel.text(), "Remote: https://example.com/repo.git");
  EXPECT_EQ(dirtyLabel.text(), "Project: Clean");
  EXPECT_EQ(commitLabel.text(), "First Push Needed");
}

TEST(StatusBarControllerTest, RefreshShowsNotConnectedWhenOriginIsMissing) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());

  EXPECT_EQ(remoteUrlLabel.text(), "Remote: Not Connected");
  EXPECT_EQ(dirtyLabel.text(), "Project: Clean");
  EXPECT_EQ(commitLabel.text(), "First Push Needed");
}

TEST(StatusBarControllerTest, RefreshShowsCurrentBranch) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  StatusBarController controller(&gitService);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{&statusBar,  &remoteUrlLabel, &branchLabel,
                                       &dirtyLabel, &commitLabel,    &refreshButton};
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());

  EXPECT_EQ(branchLabel.text(), "Branch: master");
}

TEST(StatusBarControllerTest, RefreshShowsProjectCleanWhenWorkingTreeIsClean) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{
      &statusBar, &remoteUrlLabel, &branchLabel, &dirtyLabel, &commitLabel, &refreshButton,
  };
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());
  EXPECT_EQ(dirtyLabel.text(), QString("Project: Clean"));
  EXPECT_EQ(dirtyLabel.styleSheet(), QString("color: green;"));
}

TEST(StatusBarControllerTest, RefreshShowsProjectDirtyWhenWorkingTreeHasChanges) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  // Create a new file to make the working tree dirty
  QFile newFile(tempDir.filePath("sample.configured"));
  ASSERT_TRUE(newFile.open(QIODevice::WriteOnly))
      << "Failed to create new file: " << newFile.errorString().toStdString();
  newFile.write("{\"name\":\"Sample Project\"}\n");
  newFile.close();

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{
      &statusBar, &remoteUrlLabel, &branchLabel, &dirtyLabel, &commitLabel, &refreshButton,
  };
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());
  EXPECT_EQ(dirtyLabel.text(), QString("Project: Dirty"));
  EXPECT_EQ(dirtyLabel.styleSheet(), QString("color: orange;"));
}

TEST(StatusBarControllerTest, RefreshShowsFirstPushNeededWhenNoUpstreamExists) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{
      &statusBar, &remoteUrlLabel, &branchLabel, &dirtyLabel, &commitLabel, &refreshButton,
  };
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());
  EXPECT_EQ(commitLabel.text(), QString("First Push Needed"));
}

TEST(StatusBarControllerTest, RefreshShowsUnpushedCommitsWhenAheadOfUpstream) {
  QTemporaryDir remoteDir;
  ASSERT_TRUE(remoteDir.isValid());

  QTemporaryDir localDir;
  ASSERT_TRUE(localDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QString output;

  // Create a bare remote repository.
  QProcess initBareProcess;
  initBareProcess.setWorkingDirectory(remoteDir.path());
  initBareProcess.start("git", {"init", "--bare"});
  ASSERT_TRUE(initBareProcess.waitForFinished());
  ASSERT_EQ(initBareProcess.exitCode(), 0);

  // Create the local repository.
  ASSERT_TRUE(gitService.initRepository(localDir.path(), &output)) << output.toStdString();

  ASSERT_TRUE(gitService.setConfigValue(localDir.path(), "user.name", "Test User", false, &output))
      << output.toStdString();
  ASSERT_TRUE(
      gitService.setConfigValue(localDir.path(), "user.email", "test@example.com", false, &output))
      << output.toStdString();

  ASSERT_TRUE(gitService.addRemote(localDir.path(), "origin", remoteDir.path(), &output))
      << output.toStdString();

  // Create and commit the initial project file.
  const QString projectFilePath = localDir.filePath("sample.configured");
  QFile file(projectFilePath);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("{\"name\":\"Sample Project\"}\n");
  file.close();

  ASSERT_TRUE(gitService.addAll(localDir.path(), &output)) << output.toStdString();
  ASSERT_TRUE(gitService.commit(localDir.path(), "Initial commit", &output))
      << output.toStdString();

  // Push once so the local branch gets an upstream.
  QProcess pushProcess;
  pushProcess.setWorkingDirectory(localDir.path());
  pushProcess.start("git", {"push", "-u", "origin", "HEAD"});
  ASSERT_TRUE(pushProcess.waitForFinished());
  ASSERT_EQ(pushProcess.exitCode(), 0);

  // Create one more local commit so the branch is now ahead of upstream.
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("{\"name\":\"Updated Project\"}\n");
  file.close();

  ASSERT_TRUE(gitService.addAll(localDir.path(), &output)) << output.toStdString();
  ASSERT_TRUE(gitService.commit(localDir.path(), "Second commit", &output)) << output.toStdString();

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{
      &statusBar, &remoteUrlLabel, &branchLabel, &dirtyLabel, &commitLabel, &refreshButton,
  };
  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = projectFilePath;
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());
  EXPECT_EQ(commitLabel.text(), QString("Unpushed Commits"));
  EXPECT_EQ(commitLabel.styleSheet(), QString("color: orange;"));
}

TEST(StatusBarControllerTest, RefreshShowsAllCommitsPushedWhenUpToDate) {
  QTemporaryDir remoteDir;
  ASSERT_TRUE(remoteDir.isValid());

  QTemporaryDir localDir;
  ASSERT_TRUE(localDir.isValid());

  GitService gitService;
  StatusBarController controller(&gitService);

  QString output;

  // Create a bare remote repository.
  QProcess initBareProcess;
  initBareProcess.setWorkingDirectory(remoteDir.path());
  initBareProcess.start("git", {"init", "--bare"});
  ASSERT_TRUE(initBareProcess.waitForFinished());
  ASSERT_EQ(initBareProcess.exitCode(), 0);

  // Create the local repository.
  ASSERT_TRUE(gitService.initRepository(localDir.path(), &output)) << output.toStdString();

  ASSERT_TRUE(gitService.setConfigValue(localDir.path(), "user.name", "Test User", false, &output))
      << output.toStdString();
  ASSERT_TRUE(
      gitService.setConfigValue(localDir.path(), "user.email", "test@example.com", false, &output))
      << output.toStdString();

  ASSERT_TRUE(gitService.addRemote(localDir.path(), "origin", remoteDir.path(), &output))
      << output.toStdString();

  // Create and commit the initial project file.
  const QString projectFilePath = localDir.filePath("sample.configured");
  QFile file(projectFilePath);
  ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));
  file.write("{\"name\":\"Sample Project\"}\n");
  file.close();

  ASSERT_TRUE(gitService.addAll(localDir.path(), &output)) << output.toStdString();
  ASSERT_TRUE(gitService.commit(localDir.path(), "Initial commit", &output))
      << output.toStdString();
  // Push so the local branch gets an upstream and is up to date.
  QProcess pushProcess;
  pushProcess.setWorkingDirectory(localDir.path());
  pushProcess.start("git", {"push", "-u", "origin", "HEAD"});
  ASSERT_TRUE(pushProcess.waitForFinished());
  ASSERT_EQ(pushProcess.exitCode(), 0);

  QStatusBar statusBar;
  QLabel remoteUrlLabel;
  QLabel branchLabel;
  QLabel dirtyLabel;
  QLabel commitLabel;
  QToolButton refreshButton;

  StatusBarController::Widgets widgets{
      &statusBar, &remoteUrlLabel, &branchLabel, &dirtyLabel, &commitLabel, &refreshButton,
  };

  controller.setWidgets(widgets);

  ConfiguredProject project;
  project.setGitManaged(true);

  StatusBarController::Context context;
  context.project = &project;
  context.projectFilePath = projectFilePath;
  context.hasUnsavedChanges = false;
  context.editorVisible = true;

  controller.setContext(context);
  controller.refresh();

  EXPECT_TRUE(statusBar.isVisible());
  EXPECT_TRUE(refreshButton.isVisible());
  EXPECT_EQ(commitLabel.text(), QString("All Commits Pushed"));
  EXPECT_EQ(commitLabel.styleSheet(), QString("color: green;"));
}