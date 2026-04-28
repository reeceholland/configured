#include <gtest/gtest.h>

#include <QAbstractButton>
#include <QApplication>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QString>
#include <QTemporaryDir>
#include <QTimer>

#include "app/GitWorkflowController.hpp"
#include "core/ConfiguredProject.hpp"
#include "core/ProjectService.hpp"
#include "core/git/GitService.hpp"

TEST(GitWorkflowControllerTest, ShowGitStatusWarnsWhenNoProjectIsOpen) {
  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString warningTitle;
  QString warningMessage;
  bool warningEmitted = false;

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString& title, const QString& message) {
                     warningTitle = title;
                     warningMessage = message;
                     warningEmitted = true;
                   });

  GitWorkflowController::ProjectContext context;
  context.project = nullptr;
  context.projectFilePath = QString();
  context.hasUnsavedChanges = nullptr;

  controller.setProjectContext(context);
  controller.showGitStatus();

  EXPECT_TRUE(warningEmitted);
  EXPECT_EQ(warningTitle, "Git Status");
  EXPECT_EQ(warningMessage, "Open a saved Git-managed project first.");
}

TEST(GitWorkflowControllerTest, ShowGitStatusWarnsWhenFolderIsNotARepository) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());
  QString tempPath = tempDir.path();

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString warningTitle;
  QString warningMessage;
  bool warningEmitted = false;

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString& title, const QString& message) {
                     warningTitle = title;
                     warningMessage = message;
                     warningEmitted = true;
                   });

  GitWorkflowController::ProjectContext context;
  context.project = new ConfiguredProject();
  context.projectFilePath = tempPath;
  context.hasUnsavedChanges = nullptr;

  controller.setProjectContext(context);
  controller.showGitStatus();

  EXPECT_TRUE(warningEmitted);
  EXPECT_EQ(warningTitle, "Git Status");
}

TEST(GitWorkflowControllerTest, ShowGitStatusEmitsWorkingTreeCleanMessageForEmptyStatusOutput) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  ConfiguredProject project;
  project.setGitManaged(true);

  QString infoTitle;
  QString infoMessage;
  bool infoEmitted = false;

  QObject::connect(&controller, &GitWorkflowController::informationRequested,
                   [&](const QString& title, const QString& message) {
                     infoTitle = title;
                     infoMessage = message;
                     infoEmitted = true;
                   });

  GitWorkflowController::ProjectContext context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = nullptr;

  controller.setProjectContext(context);
  controller.showGitStatus();

  EXPECT_TRUE(infoEmitted);
  EXPECT_EQ(infoTitle, QString("Git Status"));
  EXPECT_EQ(infoMessage, QString("Working tree clean."));
}

TEST(GitWorkflowControllerTest, PromptAndCommitWarnsWhenRepositoryIsMissing) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString warningTitle;
  QString warningMessage;
  bool warningEmitted = false;

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString& title, const QString& message) {
                     warningTitle = title;
                     warningMessage = message;
                     warningEmitted = true;
                   });

  ConfiguredProject project;
  project.setGitManaged(true);

  GitWorkflowController::ProjectContext context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = nullptr;

  controller.setProjectContext(context);
  controller.promptAndCommit();

  EXPECT_TRUE(warningEmitted);
  EXPECT_EQ(warningTitle, "Git Commit");
}

TEST(GitWorkflowControllerTest, PromptAndCommitWarnsWhenProjectSaveFails) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  ConfiguredProject project;
  project.createSampleProject();
  project.setGitManaged(true);

  auto first = std::make_unique<ConfiguredItem>("Param1", ConfiguredItemType::Parameter);
  first->setParameterKey("duplicate_key");
  first->setParameterValue("value1");

  auto second = std::make_unique<ConfiguredItem>("Param2", ConfiguredItemType::Parameter);
  second->setParameterKey("duplicate_key");
  second->setParameterValue("value2");

  project.root()->addChild(std::move(first));
  project.root()->addChild(std::move(second));

  bool hasUnsavedChanges = true;

  GitWorkflowController::ProjectContext context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = &hasUnsavedChanges;
  controller.setProjectContext(context);

  QString warningTitle;
  QString warningMessage;
  bool warningEmitted = false;
  bool gitStateChanged = false;

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString& title, const QString& message) {
                     warningTitle = title;
                     warningMessage = message;
                     warningEmitted = true;
                   });

  QObject::connect(&controller, &GitWorkflowController::gitStateChanged, [&]() {
    gitStateChanged = true;
  });

  QTimer::singleShot(0, []() {
    auto* box = qobject_cast<QMessageBox*>(QApplication::activeModalWidget());
    ASSERT_NE(box, nullptr);

    for (QAbstractButton* button : box->buttons()) {
      if (button->text() == "Save and Commit") {
        button->click();
        return;
      }
    }

    FAIL() << "Save and Commit button not found";
  });

  controller.promptAndCommit();

  EXPECT_TRUE(warningEmitted);
  EXPECT_EQ(warningTitle, QString("Git Commit"));
  EXPECT_TRUE(warningMessage.contains("Project could not be saved"));
  EXPECT_FALSE(gitStateChanged);
  EXPECT_TRUE(hasUnsavedChanges);
}

TEST(GitWorkflowControllerTest, PromptAndCommitEmitsGitStateChangedAfterSuccessfulCommit) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  ConfiguredProject project;
  project.createSampleProject();
  project.setGitManaged(true);

  bool hasUnsavedChanges = true;

  GitWorkflowController::ProjectContext context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = &hasUnsavedChanges;
  controller.setProjectContext(context);

  QString infoTitle;
  QString infoMessage;
  bool infoEmitted = false;
  bool warningEmitted = false;
  bool gitStateChanged = false;

  QObject::connect(&controller, &GitWorkflowController::informationRequested,
                   [&](const QString& title, const QString& message) {
                     infoTitle = title;
                     infoMessage = message;
                     infoEmitted = true;
                   });

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString&, const QString&) {
                     warningEmitted = true;
                   });

  QObject::connect(&controller, &GitWorkflowController::gitStateChanged, [&]() {
    gitStateChanged = true;
  });

  // Track which modal dialogs we have already handled so we can drive the
  // multi-step commit flow in the correct order.
  bool handledSavePrompt = false;
  bool handledCommitPrompt = false;

  QTimer timer;
  QObject::connect(&timer, &QTimer::timeout, [&]() {
    // promptAndCommit() is synchronous and opens modal dialogs.
    // While those dialogs are running, Qt still processes events, so this
    // timer can observe the active modal widget and interact with it.
    QWidget* modal = QApplication::activeModalWidget();
    if (!modal) {
      return;
    }

    // Handle the preflight message box that appears when there are
    // unsaved editor changes. We explicitly click "Save and Commit" so the
    // controller saves the project and then continues to the commit message step.
    if (!handledSavePrompt) {
      auto* box = qobject_cast<QMessageBox*>(modal);
      if (box) {
        for (QAbstractButton* button : box->buttons()) {
          if (button->text() == "Save and Commit") {
            handledSavePrompt = true;
            button->click();
            return;
          }
        }
      }
    }

    // After the save-preflight dialog closes, the controller opens a
    // QInputDialog for the commit message. Once it becomes the active modal
    // widget, fill in a message and accept it so the commit can proceed.
    if (!handledCommitPrompt) {
      auto* input = qobject_cast<QInputDialog*>(modal);
      if (input) {
        handledCommitPrompt = true;
        input->setTextValue("Test commit");
        input->accept();
        return;
      }
    }
  });

  // Poll frequently so the test reacts quickly when each modal dialog appears.
  timer.start(10);

  // This call blocks until the full commit flow completes, but the timer above
  // continues running inside Qt's event loop and drives the dialogs for us.
  controller.promptAndCommit();

  timer.stop();

  // Confirm both expected UI steps occurred.
  EXPECT_TRUE(handledSavePrompt);
  EXPECT_TRUE(handledCommitPrompt);

  // A successful commit should update Git state and emit an informational message.
  EXPECT_TRUE(gitStateChanged);
  EXPECT_TRUE(infoEmitted);
  EXPECT_FALSE(warningEmitted);
  EXPECT_EQ(infoTitle, QString("Git Commit"));
  EXPECT_FALSE(infoMessage.isEmpty());

  // The save-first path should clear the unsaved editor flag.
  EXPECT_FALSE(hasUnsavedChanges);

  // The controller updates the project with the new commit hash after a
  // successful commit, so this should now be populated.
  EXPECT_FALSE(project.gitCommitHash().trimmed().isEmpty());

  // The repository should be clean because the project was saved,
  // staged, and committed successfully.
  bool clean = false;
  ASSERT_TRUE(gitService.workingTreeClean(tempDir.path(), &clean, &output)) << output.toStdString();
  EXPECT_TRUE(clean);
}

TEST(GitWorkflowControllerTest, PromptAndSwitchBranchRejectsUnsavedEditorChanges) {
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  GitService gitService;
  ProjectService projectService(&gitService);
  GitWorkflowController controller(&gitService, &projectService);

  QString output;
  ASSERT_TRUE(gitService.initRepository(tempDir.path(), &output)) << output.toStdString();

  ConfiguredProject project;
  project.setGitManaged(true);

  bool hasUnsavedChanges = true;

  GitWorkflowController::ProjectContext context;
  context.project = &project;
  context.projectFilePath = tempDir.filePath("sample.configured");
  context.hasUnsavedChanges = &hasUnsavedChanges;
  controller.setProjectContext(context);

  QString warningTitle;
  QString warningMessage;
  bool warningEmitted = false;
  bool reloadRequested = false;
  bool gitStateChanged = false;

  QObject::connect(&controller, &GitWorkflowController::warningRequested,
                   [&](const QString& title, const QString& message) {
                     warningTitle = title;
                     warningMessage = message;
                     warningEmitted = true;
                   });

  QObject::connect(&controller, &GitWorkflowController::reloadProjectRequested,
                   [&](const QString&) {
                     reloadRequested = true;
                   });

  QObject::connect(&controller, &GitWorkflowController::gitStateChanged, [&]() {
    gitStateChanged = true;
  });

  controller.promptAndSwitchBranch();

  EXPECT_TRUE(warningEmitted);
  EXPECT_EQ(warningTitle, QString("Switch Branch"));
  EXPECT_TRUE(warningMessage.contains("Save or commit your changes before switching branches."));
  EXPECT_FALSE(reloadRequested);
  EXPECT_FALSE(gitStateChanged);
  EXPECT_TRUE(hasUnsavedChanges);
}
