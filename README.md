# CONFIGURED

**Robotics Configuration Studio**

CONFIGURED is a desktop application built with C++ and Qt for designing, validating, and managing structured robotics configuration data.

It provides a model-driven interface for constructing hierarchical system configurations, enabling consistency, traceability, and maintainability across complex robotic platforms.

---

## Status

- Actively developed
- Stable core editing workflow
- Production-oriented architecture under continuous refinement

---

## Overview

CONFIGURED addresses the limitations of manually editing configuration files (e.g. JSON/YAML) by introducing a structured tooling layer for:

- Deterministic configuration authoring
- Real-time validation
- Metadata management
- Controlled editing workflows

The application is designed for robotics and autonomous systems where configuration correctness is critical.

---

## Key Features

### Hierarchical Configuration Model
- System → Subsystem → Component → Parameter structure
- Tree-based representation for scalable system design
- Explicit parent-child relationships

### Parameter Editing
- Key/value configuration model
- Support for units and required constraints
- Designed for future type safety extensions

### Validation
- Detection of duplicate parameter keys
- Enforcement of required parameters
- Foundation for rule-based validation framework

### Project Persistence
- Save and load `.configured` project files
- Structured storage of configuration and metadata
- Designed for future database-backed storage

### Metadata Management
- Project metadata includes:
  - Name, description, author, company
  - Version and target platform
- Automatic last modified timestamp tracking

### User Interface
- Built with Qt (Widgets)
- Model-driven UI reflecting underlying data
- Structured and constrained editing workflow

---

## Why CONFIGURED

Manual configuration editing introduces risk:

- No validation guarantees
- Prone to duplication and inconsistency
- Poor scalability for large systems

CONFIGURED provides:

- Structured editing with constraints
- Centralised configuration management
- A foundation for integration with runtime systems (e.g. ROS 2)

---

## Architecture

CONFIGURED follows a layered architecture with clear separation of concerns:

```text
UI Layer (Qt Widgets)
    ↓
MainWindow (Application Controller)
    ↓
EditorScreenWidget (Interaction Layer)
    ↓
ConfiguredProject (Project Model)
    ↓
ConfiguredItem (Hierarchical Data Structure)
```

### Core Components

| Component | Responsibility |
|-----------|----------------|
| ConfiguredProject | Project state and metadata management |
| ConfiguredItem | Hierarchical configuration node representation |
| EditorScreenWidget | Primary editing interface |
| HomeScreenWidget | Entry point and navigation |
| ProjectMetadataDialog | Metadata editing interface |

---

## Project Structure

```text
configured/
├── include/
│   ├── app/
│   ├── core/
│   └── ui/
├── src/
│   ├── app/
│   ├── core/
│   └── ui/
├── resources/
│   └── images/
├── CMakeLists.txt
└── README.md
```

---

## Example Configuration

```json
{
  "projectName": "My Rover",
  "version": "0.1.0",
  "robotPlatform": "Rugged Rover",
  "lastModified": "2026-04-11 10:15:00",
  "root": {
    "name": "System",
    "type": "System",
    "children": [
      {
        "name": "Drivetrain",
        "type": "Subsystem",
        "children": [
          {
            "name": "Left Motor",
            "type": "Component",
            "children": [
              {
                "name": "Max RPM",
                "type": "Parameter",
                "parameterKey": "max_rpm",
                "parameterValue": "150",
                "parameterUnit": "rpm",
                "required": true
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

## Build Instructions

### Prerequisites

- CMake ≥ 3.21
- Qt 6 (Core, Widgets, GUI)
- Ninja or Visual Studio

### Build

```bash
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

### Run

```bash
out/build/x64-debug/Configured.exe
```

---

## Design Principles

- Model-driven architecture with a single source of truth
- Strong separation between UI and data model
- Deterministic configuration editing
- Extensible for future robotics workflows

---

## Roadmap

### Implemented
- Hierarchical configuration system
- Parameter editing and validation (duplicate keys)
- Project metadata management
- Structured project persistence

### Planned
- Advanced validation framework (error panel and highlighting)
- ROS 2 YAML export
- Git integration and version tracking
- Undo/redo system
- Plugin-based configuration extensions

---

## Future Direction

CONFIGURED is designed to evolve into a full configuration pipeline tool, supporting:

- Export to ROS 2 parameter and launch systems
- Integration with version control workflows
- Validation against runtime schemas
- Plugin-based domain extensions

---

## Contributing

Contributions, ideas, and feedback are welcome.  
The project is under active development and open to iteration.

---

## License

To be defined

---

## Author

Reece Holland  
Software Engineer – Robotics and Autonomous Systems