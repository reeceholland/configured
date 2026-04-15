# CONFIGURED

**Robotics Configuration Studio**

CONFIGURED is a desktop application built with **C++ and Qt** for designing, editing, and managing structured robotics configuration files.

It provides a visual interface for defining robot systems, components, and parameters, with validation and extensibility in mind.

---

## 🚀 Features

* 🧱 **Hierarchical Configuration Editor**

  * System → Subsystems → Components → Parameters
* 📝 **Parameter Editing**

  * Key, value, unit, required flag
* 🧠 **Validation**

  * Detect duplicate parameter keys
* 💾 **Project Persistence**

  * Save/load `.configured` JSON files
* 🧾 **Project Metadata**

  * Name, description, author, version, platform
  * Auto-updated *Last Modified* timestamp
* 🎨 **Modern UI**

  * Custom home screen with background image
  * Glass-style panels and dark theme
* 🛠️ **Toolbar Actions**

  * Add/remove items
  * Save/load projects
  * Edit metadata

---

## 🏗️ Architecture

The project follows a clean separation of concerns:

```
UI (Qt Widgets)
    ↓
MainWindow (Controller)
    ↓
EditorScreenWidget (View + Interaction)
    ↓
ConfiguredProject (Model)
    ↓
ConfiguredItem (Tree Structure)
```

### Core Components

| Component               | Description                                |
| ----------------------- | ------------------------------------------ |
| `ConfiguredProject`     | Owns project data and metadata             |
| `ConfiguredItem`        | Represents nodes in the configuration tree |
| `EditorScreenWidget`    | Main editor UI                             |
| `HomeScreenWidget`      | Entry screen                               |
| `ProjectMetadataDialog` | Metadata editing dialog                    |

---

## 📦 Project Structure

```
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

## 🧪 Example Configuration

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

## 🛠️ Build Instructions

### Prerequisites

* CMake ≥ 3.21
* Qt 6 (Widgets, Core, Gui)
* Ninja (recommended) or Visual Studio

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

## 🎯 Design Goals

* Provide a **structured alternative to raw JSON/YAML editing**
* Enable **validation and consistency** in robotics configs
* Support **future export to ROS 2 / YAML**
* Be extensible for:

  * Git versioning
  * Validation frameworks
  * Plugin-based configuration types

---

## 🔮 Roadmap

* ✅ Parameter validation (duplicate keys)
* 🔲 Advanced validation system (error panel + highlighting)
* 🔲 ROS 2 YAML export
* 🔲 Git integration
* 🔲 Undo/Redo system
* 🔲 Plugin architecture

---

## 🧠 Key Concepts

* **Model-driven design**: UI reflects underlying data model
* **Signal/slot architecture**: decoupled UI interactions
* **Tree-based configuration**: scalable structure for complex robots

---

## 🤝 Contributing

This project is under active development. Contributions, ideas, and feedback are welcome.

---

## 📄 License

TBD

---

## 👤 Author

Reece Holland
Software Engineer – Robotics & Autonomous Systems

---
