#pragma once

#include <QHash>
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

  void setProject(ConfiguredProject* project);

  ConfiguredProject* project();
  const ConfiguredProject* project() const;

  void addChildToSelected();
  void removeSelectedItem();

  void setProjectName(const QString& name);

  void updateParameterValidationUi();

  void refreshTree();

 signals:
  void projectModified(ConfiguredItem* item);

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

  ConfiguredProject* project_ = nullptr;
  ConfiguredItem* selectedItem_ = nullptr;
  bool updatingUi_ = false;

  void rebuildTree();
  void addTreeItemRecursive(QTreeWidgetItem* parentItem, ConfiguredItem* item);

  void onTreeSelectionChanged(QTreeWidgetItem* current);
  void loadSelectedItemIntoEditor();
  void applyEditorToSelectedItem();
  void setValidationState(QWidget* field, QLabel* errorLabel, const QString& errorText);

  void applyTreeItemState(QTreeWidgetItem* treeItem, const ConfiguredItem* item);
  void refreshTreeItemState(ConfiguredItem* item);

  QLabel* parameterKeyErrorLabel_ = nullptr;
  QLabel* parameterValueErrorLabel_ = nullptr;
  QLabel* itemNameErrorLabel_ = nullptr;

  QHash<ConfiguredItem*, QTreeWidgetItem*> itemToTreeItem_;
};