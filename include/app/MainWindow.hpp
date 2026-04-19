/**
 * @file MainWindow.hpp
 * @author Reece Holland
 * @brief Main window of the Configured application.
 * @version 0.3
 * @date 2026-04-15
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QMainWindow>

#include "core/GitService.hpp"
#include "core/ProjectService.hpp"

class QStackedWidget;
class QAction;
class QToolBar;
class QToolButton;
class QMenu;
class HomeScreenWidget;
class EditorScreenWidget;
class HelpScreenWidget;

/**
 * @brief Main window of the Configured application.
 *
 * The MainWindow class manages the main user interface of the application,
 * including the home screen, editor screen, and help screen. It also handles
 * actions related to project management, Git integration, and exporting project
 * parameters. The main window uses a QStackedWidget to switch between different
 * screens and a QToolBar for quick access to common actions.
 *
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  /**
   * @brief Construct a new Main Window object
   *
   * Initializes the main window, sets up the user interface, and connects signals
   * and slots for various actions.
   */
  MainWindow();
  ~MainWindow() override;

 private:
  /**
   * @brief Show the home screen.
   *
   * Switches the main window to display the home screen.
   */
  void showHome();

  /**
   * @brief Show the editor screen.
   *
   * Switches the main window to display the editor screen.
   */
  void showEditor();

  /**
   * @brief Show the help screen.
   *
   * Switches the main window to display the help screen.
   */
  void showHelp();

  /**
   * @brief Enable or disable editor-related actions.
   *
   * @param enabled If true, editor actions will be enabled; otherwise, they will be disabled.
   */
  void setEditorActionsEnabled(bool enabled);

  /**
   * @brief Update the window title.
   *
   * Updates the main window's title based on the current project and state.
   */
  void updateWindowTitle();

  /**
   * @brief Prompt the user to create a new project.
   *
   * Displays a dialog to gather information for a new project and initializes it.
   */
  void promptAndCreateProject();

  /**
   * @brief Update the visibility of Git-related UI elements.
   *
   * Shows or hides Git-related actions and buttons based on the current project state.
   */
  void updateGitUiVisibility();

  /**
   * @brief Get the working directory of the current project.
   *
   * @return QString The absolute path to the directory containing the current project
   * file, or an empty string if no project is loaded.
   *
   */
  QString currentProjectWorkingDirectory() const;

  /**
   * @brief Export the current project's parameters to a JSON file.
   *
   * Prompts the user to select a file path and exports the parameters of the
   * currently loaded project in JSON format. Displays an error message if no
   * project is loaded or if the export fails.
   */
  void exportParametersToJson();

  /**
   * @brief Export the current project's parameters to an XML file.
   *
   * Prompts the user to select a file path and exports the parameters of the
   * currently loaded project in XML format. Displays an error message if no
   * project is loaded or if the export fails.
   */

  void exportParametersToXml();

  /**
   * @brief Refresh the Git metadata for the current project.
   *
   * Updates the Git-related metadata (e.g., commit hash) for the currently
   * loaded project. This is typically called after saving the project to ensure
   * that the latest Git information is associated with it.
   */
  void refreshProjectGitMetadata();

  /**
   * @brief Edit the metadata of the current project.
   *
   * Opens a dialog to allow the user to edit the metadata of the currently
   * loaded project, such as name, description, author, and Git information. After
   * the user saves their changes, the project metadata is updated and saved.
   */
  void editProjectMetadata();

  void promptAndCloneRemoteProject();

  QString findConfiguredFile(const QString& folderPath) const;

 private slots:

  /**
   * @brief Handle the Git status action.
   *
   * This slot is called when the user triggers the Git status action. It retrieves
   * and displays the current Git status for the project.
   */
  void onGitStatus();

  /**
   * @brief Handle the Git commit action.
   *
   * This slot is called when the user triggers the Git commit action. It commits
   * the current changes to the Git repository.
   */
  void onGitCommit();

 private:
  /// @brief The main stacked widget that holds the different screens (home, editor, help).
  QStackedWidget* stack_ = nullptr;

  /// @brief The home screen widget.
  HomeScreenWidget* home_ = nullptr;

  /// @brief The editor screen widget.
  EditorScreenWidget* editor_ = nullptr;

  /// @brief The help screen widget.
  HelpScreenWidget* help_ = nullptr;

  /// @brief The main toolbar.
  QToolBar* toolbar_ = nullptr;

  /// @brief The Git button in the toolbar.
  QToolButton* gitButton_ = nullptr;

  /// @brief The save project action.
  QAction* saveProjectAction_ = nullptr;

  /// @brief The add child action.
  QAction* addChildAction_ = nullptr;

  /// @brief The remove item action.
  QAction* removeItemAction_ = nullptr;

  /// @brief The go home action.
  QAction* goHomeAction_ = nullptr;

  /// @brief The project metadata action.
  QAction* projectMetadataAction_ = nullptr;

  /// @brief The version information action.
  QAction* versionAction_ = nullptr;

  /// @brief The help action.
  QAction* helpAction_ = nullptr;

  /// @brief The Git button action.
  QAction* gitButtonAction_ = nullptr;

  /// @brief The export to XML action.
  QAction* exportXmlAction_ = nullptr;

  /// @brief The export to JSON action.
  QAction* exportJsonAction_ = nullptr;

  /// @brief The Git initialization action.
  QAction* gitInitAction_ = nullptr;

  /// @brief The Git status action.
  QAction* gitStatusAction_ = nullptr;

  /// @brief The Git commit action.
  QAction* gitCommitAction_ = nullptr;

  /// @brief The Git configuration action.
  QAction* gitConfigAction_ = nullptr;

  /// @brief The export action.
  QAction* exportAction_ = nullptr;

  QWidget* lastCentral_ = nullptr;

  /// @brief The Git service for handling Git operations.
  GitService gitService_;

  ProjectService projectService_{&gitService_};

  std::unique_ptr<ConfiguredProject> currentProject_;

  QString currentProjectFilePath_;
};