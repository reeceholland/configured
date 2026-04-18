#pragma once

#include <QWidget>
#include <memory>

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredProject.hpp"

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QCheckBox;
class QWidget;
class ConfiguredProject;
class QLabel;

class EditorScreenWidget : public QWidget {
  Q_OBJECT

 public:
  explicit EditorScreenWidget(QWidget* parent = nullptr);

  void createNewProject();
  void addChildToSelected();
  void removeSelectedItem();

  void setProjectName(const QString& name);
  bool saveProject(const QString& filePath);
  bool loadProject(const QString& filePath);

  QString currentFilePath() const;
  bool hasProjectFilePath() const;

  void updateParameterValidationUi();

  ConfiguredProject* project();
  const ConfiguredProject* project() const;

  void setProject(std::unique_ptr<ConfiguredProject> project, const QString& filePath = QString());

 private:
  QTreeWidget* tree_ = nullptr;

  QLineEdit* nameEdit_ = nullptr;
  QComboBox* typeCombo_ = nullptr;
  QTextEdit* descriptionEdit_ = nullptr;

  QWidget* parameterPanel_ = nullptr;
  QLineEdit* parameterKeyEdit_ = nullptr;
  QLineEdit* parameterValueEdit_ = nullptr;
  QLineEdit* parameterUnitEdit_ = nullptr;
  QCheckBox* requiredCheck_ = nullptr;

  QString currentFilePath_;

  std::unique_ptr<ConfiguredProject> project_;
  ConfiguredItem* selectedItem_ = nullptr;
  bool updatingUi_ = false;

  void rebuildTree();
  void addTreeItemRecursive(QTreeWidgetItem* parentItem, ConfiguredItem* item);

  void onTreeSelectionChanged(QTreeWidgetItem* current);
  void loadSelectedItemIntoEditor();
  void applyEditorToSelectedItem();
  void setValidationState(QWidget* field, QLabel* errorLabel, const QString& errorText);

  QString typeToString(ConfiguredItemType type) const;
  ConfiguredItemType stringToType(const QString& text) const;

  QLabel* parameterKeyErrorLabel_ = nullptr;
  QLabel* parameterValueErrorLabel_ = nullptr;
  QLabel* itemNameErrorLabel_ = nullptr;
};