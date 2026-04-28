# CONFIGURED

**Robotics Configuration Studio**

CONFIGURED is a C++17 and Qt 6 desktop application for creating, validating, saving, exporting, and version-managing structured robotics configuration projects.

The app provides a guided UI for building hierarchical configuration data without manually editing JSON, YAML, or XML files.

---

## Status

CONFIGURED is under active development.

Current focus areas:

- project save/load workflow
- validation
- Git-backed project workflows
- remote clone/pull support
- maintainable C++/Qt architecture

---

## Features

### Structured Project Editing

CONFIGURED models a project as a hierarchy:

```text
System
  Subsystem
    Component
      Parameter
```

Each configuration item can store:

- name
- type
- description
- child items

Parameter items can also store:

- key
- value
- unit
- required flag

### Project Persistence

Projects are saved as `.configured` files.

A project folder typically looks like:

```text
MyProject/
  MyProject.configured
  .git/
```

The `.configured` file stores project metadata and configuration data in structured JSON.

### Validation

The app validates project data before saving.

Current validation includes:

- duplicate parameter key detection
- required parameter key checks
- required parameter value checks
- invalid item name checks
- whole-project validation before save

### Metadata Management

Project metadata includes:

- project name
- description
- author
- company
- version
- robot platform
- Git-managed flag
- last modified timestamp

### Export

Project parameters can be exported to:

- JSON
- XML

### Git Integration

CONFIGURED includes project-focused Git workflows:

- initialize Git-managed projects
- show Git status
- commit project changes
- connect a remote repository
- clone a remote project from the home screen
- pull remote changes
- show current branch
- show remote URL

Long-running Git operations such as clone and pull run through worker objects so the UI remains responsive.

---

## Architecture

CONFIGURED uses a layered structure with clear ownership boundaries.

```text
MainWindow
  owns the active project session
  owns the current project file path
  coordinates user workflows

EditorScreenWidget
  displays and edits the active project
  borrows the project pointer
  does not save/load projects

ProjectService
  creates, loads, saves, and validates projects

ConfiguredProject
  owns project metadata and the root configuration item
  serializes/deserializes project files

ConfiguredItem
  represents one node in the configuration hierarchy

GitService
  wraps low-level Git commands

CloneWorker / GitPullWorker
  run long Git operations outside the GUI thread
```

---

## Core Components

| Component             | Responsibility                                                           |
| --------------------- | ------------------------------------------------------------------------ |
| `MainWindow`          | Main application controller, screen switching, project session ownership |
| `HomeScreenWidget`    | Entry screen for creating, opening, and connecting projects              |
| `EditorScreenWidget`  | Tree editor and item property editing                                    |
| `ConfiguredProject`   | Project model, metadata, JSON persistence                                |
| `ConfiguredItem`      | Hierarchical configuration item model                                    |
| `ProjectService`      | Project create/load/save/update workflows                                |
| `ProjectValidator`    | Whole-project validation before save                                     |
| `GitService`          | Low-level Git command wrapper                                            |
| `CloneWorker`         | Asynchronous remote clone worker                                         |
| `GitPullWorker`       | Asynchronous pull worker                                                 |
| `JsonProjectExporter` | JSON parameter export                                                    |
| `XmlProjectExporter`  | XML parameter export                                                     |

---

## Project Structure

```text
configured/
  include/
    app/
    core/
      validation/
    export/
    ui/
  src/
    app/
    core/
      validation/
    export/
    ui/
  tests/
  resources/
  docs/
  CMakeLists.txt
  CMakePresets.json
  README.md
```

---

## Requirements

- C++17 compiler
- CMake 3.21 or newer
- Ninja
- Qt 6 with:
  - Core
  - Gui
  - Widgets
- Git
- Doxygen, optional, for documentation generation

On Windows, the current presets expect Qt at:

```text
C:/Qt/6.10.1/msvc2022_64
```

---

## Configure And Build

From the repository root:

```powershell
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Release build:

```powershell
cmake --preset x64-release
cmake --build out/build/x64-release
```

---

## Run

Debug build:

```powershell
out/build/x64-debug/Configured.exe
```

Release build:

```powershell
out/build/x64-release/Configured.exe
```

---

## Run Tests

Build first:

```powershell
cmake --build out/build/x64-debug
```

Run all tests:

```powershell
ctest --test-dir out/build/x64-debug --output-on-failure
```

Run a specific test:

```powershell
ctest --test-dir out/build/x64-debug --output-on-failure -R ProjectServiceTest
```

Current test areas include:

- configured item behavior
- project save/load round trips
- project service load/save workflow
- project-wide validation
- duplicate parameter key detection

---

## Generate Documentation

If Doxygen is installed:

```powershell
cmake --build out/build/x64-debug --target doc
```

Generated documentation is written under the build directory:

```text
out/build/x64-debug/docs
```

---

## Git Workflows

### Local Git-Managed Project

A project can be created as Git-managed. The project folder becomes the Git working directory.

```text
ProjectFolder/
  ProjectName.configured
  .git/
```

### Connect Remote

For an open Git-managed project, the Git menu can connect the local repository to a remote URL.

### Clone Remote Project

The home screen can clone a remote repository locally. After clone completes, CONFIGURED searches for a `.configured` file and opens it in the editor.

Clone runs in a worker thread so the UI does not freeze.

### Pull Remote Changes

Git pull is treated as a long-running operation and should run outside the GUI thread.

---

## Configuration File Example

```json
{
  "projectName": "My Rover",
  "description": "Example robotics configuration",
  "author": "Reece Holland",
  "company": "Example Co",
  "version": "0.4.0",
  "lastModified": "2026-04-21T10:15:00",
  "robotPlatform": "Rugged Rover",
  "gitManaged": true,
  "root": {
    "name": "System",
    "type": "System",
    "description": "Top-level system configuration.",
    "children": [
      {
        "name": "Drivetrain",
        "type": "Subsystem",
        "description": "",
        "children": [
          {
            "name": "LeftMotor",
            "type": "Component",
            "description": "",
            "children": [
              {
                "name": "MaxRPM",
                "type": "Parameter",
                "parameterKey": "max_rpm",
                "parameterValue": "150",
                "parameterUnit": "rpm",
                "required": true,
                "children": []
              }
            ]
          }
        ]
      }
    ]
  }
}
```

---

## Development Notes

### Ownership Model

`MainWindow` owns the active project:

```cpp
std::unique_ptr<ConfiguredProject> currentProject_;
QString currentProjectFilePath_;
```

`EditorScreenWidget` receives a non-owning pointer:

```cpp
editor_->setProject(currentProject_.get());
```

This keeps persistence and session ownership out of the editor widget.

### Save Workflow

The save path is:

```text
MainWindow
  ProjectService::saveProject
    ProjectValidator::validate
    ConfiguredProject::saveToFile
```

### Long-Running Git Commands

Long-running Git operations should not block the Qt GUI thread. Use a worker object and `QThread` for commands such as:

- clone
- pull
- push, if network latency becomes noticeable

---

## Roadmap

Planned or likely future work:

- dirty-state tracking for unsaved changes
- Save As workflow
- improved Git pull/push UX
- clearer merge conflict handling
- branch/upstream status display
- remote URL management
- async push support
- stronger typed parameter values
- undo/redo
- ROS 2 YAML export
- richer validation summaries
- plugin-based configuration extensions

---

## Contributing

Contributions, ideas, and feedback are welcome.

Recommended development loop:

```powershell
cmake --build out/build/x64-debug
ctest --test-dir out/build/x64-debug --output-on-failure
```

Please keep changes focused and add tests for project service, validation, persistence, or Git workflow behavior where practical.

---

## License

To be defined.

---

## Author

Reece Holland  
Software Engineer - Robotics and Autonomous Systems
