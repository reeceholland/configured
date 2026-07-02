#include "ui/EditorScreenWidget.hpp"

#include <QBrush>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSplitter>
#include <QTextEdit>
#include <QToolTip>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QVariant>

#include "core/ConfiguredItem.hpp"
#include "core/ConfiguredItemTypes.hpp"
#include "core/ConfiguredProject.hpp"
#include "core/validation/item/ItemValidator.hpp"

namespace {
int countParameterKeyOccurrences(const ConfiguredItem* item, const QString& key) {
  if (!item || key.trimmed().isEmpty()) {
    return 0;
  }

  int count = 0;

  if (item->isParameter() && item->parameterKey().trimmed() == key.trimmed()) {
    ++count;
  }

  for (const auto& child : item->children()) {
    count += countParameterKeyOccurrences(child.get(), key);
  }

  return count;
}
}  // namespace

namespace {
QString firstErrorForField(const ValidationResult& result, const QString& field) {
  for (const ValidationMessage& message : result.messages()) {
    if (message.field == field && message.severity == ValidationSeverity::Error) {
      return message.message;
    }
  }
  return "";
}
}  // namespace

EditorScreenWidget::EditorScreenWidget(QWidget* parent) : QWidget(parent) {
  auto* layout = new QHBoxLayout(this);

  auto* splitter = new QSplitter(this);

  tree_ = new QTreeWidget(splitter);
  tree_->setHeaderLabel("Configuration");

  auto* rightPanel = new QWidget(splitter);
  auto* rightLayout = new QVBoxLayout(rightPanel);

  auto* title = new QLabel("Properties", rightPanel);
  QFont titleFont = title->font();
  titleFont.setPointSize(14);
  titleFont.setBold(true);
  title->setFont(titleFont);

  auto* form = new QFormLayout();

  nameEdit_ = new QLineEdit(rightPanel);

  typeCombo_ = new QComboBox(rightPanel);
  typeCombo_->addItems({"System", "Subsystem", "Component", "Parameter"});

  descriptionEdit_ = new QTextEdit(rightPanel);
  descriptionEdit_->setMinimumHeight(120);

  itemNameErrorLabel_ = new QLabel(this);
  itemNameErrorLabel_->setStyleSheet("color: #ff6b6b;");
  itemNameErrorLabel_->setVisible(false);

  form->addRow("Name:", nameEdit_);
  form->addRow("", itemNameErrorLabel_);

  form->addRow("Type:", typeCombo_);
  form->addRow("Description:", descriptionEdit_);

  parameterPanel_ = new QWidget(rightPanel);
  auto* parameterForm = new QFormLayout(parameterPanel_);

  parameterKeyEdit_ = new QLineEdit(parameterPanel_);
  parameterValueEdit_ = new QLineEdit(parameterPanel_);
  parameterUnitEdit_ = new QLineEdit(parameterPanel_);
  requiredCheck_ = new QCheckBox("Required", parameterPanel_);

  parameterKeyErrorLabel_ = new QLabel(this);
  parameterKeyErrorLabel_->setStyleSheet("color: #ff6b6b;");
  parameterKeyErrorLabel_->setVisible(false);

  parameterValueErrorLabel_ = new QLabel(this);
  parameterValueErrorLabel_->setStyleSheet("color: #ff6b6b;");
  parameterValueErrorLabel_->setVisible(false);

  parameterForm->addRow("Parameter Key:", parameterKeyEdit_);
  parameterForm->addRow("", parameterKeyErrorLabel_);

  parameterForm->addRow("Parameter Value:", parameterValueEdit_);
  parameterForm->addRow("", parameterValueErrorLabel_);

  parameterForm->addRow("Unit:", parameterUnitEdit_);
  parameterForm->addRow("", requiredCheck_);

  parameterForm->addRow("", itemNameErrorLabel_);
  rightLayout->addWidget(title);
  rightLayout->addLayout(form);
  rightLayout->addWidget(parameterPanel_);
  rightLayout->addStretch();

  splitter->addWidget(tree_);
  splitter->addWidget(rightPanel);
  splitter->setStretchFactor(1, 1);

  layout->addWidget(splitter);

  connect(tree_, &QTreeWidget::currentItemChanged, this,
          [this](QTreeWidgetItem* current, QTreeWidgetItem*) {
            onTreeSelectionChanged(current);
          });

  connect(nameEdit_, &QLineEdit::textEdited, this, [this](const QString&) {
    applyEditorToSelectedItem();
  });

  connect(typeCombo_, &QComboBox::currentTextChanged, this, [this](const QString&) {
    applyEditorToSelectedItem();
  });

  connect(descriptionEdit_, &QTextEdit::textChanged, this, [this]() {
    applyEditorToSelectedItem();
  });

  connect(parameterKeyEdit_, &QLineEdit::textEdited, this, [this](const QString&) {
    applyEditorToSelectedItem();
  });

  connect(parameterValueEdit_, &QLineEdit::textEdited, this, [this](const QString&) {
    applyEditorToSelectedItem();
  });

  connect(parameterUnitEdit_, &QLineEdit::textEdited, this, [this](const QString&) {
    applyEditorToSelectedItem();
  });

  connect(requiredCheck_, &QCheckBox::toggled, this, [this](bool) {
    applyEditorToSelectedItem();
  });
}

void EditorScreenWidget::addChildToSelected() {
  if (!selectedItem_) {
    return;
  }

  ConfiguredItemType childType = ConfiguredItemType::Component;
  QString childName = "New Item";

  switch (selectedItem_->type()) {
    case ConfiguredItemType::System:
      childType = ConfiguredItemType::Subsystem;
      childName = "New-Subsystem";
      break;
    case ConfiguredItemType::Subsystem:
      childType = ConfiguredItemType::Component;
      childName = "New-Component";
      break;
    case ConfiguredItemType::Component:
      childType = ConfiguredItemType::Parameter;
      childName = "New-Parameter";
      break;
    case ConfiguredItemType::Parameter:
      childType = ConfiguredItemType::Parameter;
      childName = "New-Parameter";
      break;
  }

  auto child = std::make_unique<ConfiguredItem>(childName, childType);
  child->setDescription("Newly created item.");

  if (childType == ConfiguredItemType::Parameter) {
    child->setParameterKey("new_key");
    child->setParameterValue("value");
    child->setParameterUnit("");
    child->setRequired(false);
  }

  selectedItem_->addChild(std::move(child));

  selectedItem_->setIsDirty(true);
  emit projectModified(selectedItem_);

  rebuildTree();
}

void EditorScreenWidget::removeSelectedItem() {
  if (!selectedItem_) {
    return;
  }

  if (!project_ || selectedItem_ == project_->root()) {
    return;
  }

  ConfiguredItem* parent = selectedItem_->parent();
  if (!parent) {
    return;
  }

  parent->removeChild(selectedItem_);
  parent->setIsDirty(true);
  emit projectModified(parent);
  selectedItem_ = nullptr;
  rebuildTree();
}

void EditorScreenWidget::rebuildTree() {
  tree_->clear();
  itemToTreeItem_.clear();

  if (!project_ || !project_->root()) {
    return;
  }

  auto* rootItem = new QTreeWidgetItem(tree_);
  rootItem->setText(0, project_->root()->name());
  rootItem->setData(0, Qt::UserRole,
                    QVariant::fromValue(reinterpret_cast<quintptr>(project_->root())));
  itemToTreeItem_[project_->root()] = rootItem;

  applyTreeItemState(rootItem, project_->root());

  for (auto& child : project_->root()->children()) {
    addTreeItemRecursive(rootItem, child.get());
  }

  tree_->expandAll();
  tree_->setCurrentItem(rootItem);
}

void EditorScreenWidget::addTreeItemRecursive(QTreeWidgetItem* parentItem, ConfiguredItem* item) {
  if (!parentItem || !item) {
    return;
  }

  auto* treeItem = new QTreeWidgetItem(parentItem);
  treeItem->setText(0, item->name());
  treeItem->setData(0, Qt::UserRole, QVariant::fromValue(reinterpret_cast<quintptr>(item)));

  itemToTreeItem_[item] = treeItem;
  applyTreeItemState(treeItem, item);

  for (auto& child : item->children()) {
    addTreeItemRecursive(treeItem, child.get());
  }
}

void EditorScreenWidget::onTreeSelectionChanged(QTreeWidgetItem* current) {
  if (!current) {
    selectedItem_ = nullptr;
    loadSelectedItemIntoEditor();
    return;
  }

  selectedItem_ =
      reinterpret_cast<ConfiguredItem*>(current->data(0, Qt::UserRole).value<quintptr>());

  loadSelectedItemIntoEditor();
}

void EditorScreenWidget::loadSelectedItemIntoEditor() {
  updatingUi_ = true;

  if (!selectedItem_) {
    nameEdit_->clear();
    typeCombo_->setCurrentIndex(0);
    descriptionEdit_->clear();

    parameterKeyEdit_->clear();
    parameterValueEdit_->clear();
    parameterUnitEdit_->clear();
    requiredCheck_->setChecked(false);
    parameterPanel_->setVisible(false);

    updatingUi_ = false;
    return;
  }

  nameEdit_->setText(selectedItem_->name());
  typeCombo_->setCurrentText(configuredItemTypeToString(selectedItem_->type()));
  descriptionEdit_->setPlainText(selectedItem_->description());

  const bool isParameter = selectedItem_->isParameter();
  parameterPanel_->setVisible(isParameter);

  parameterKeyEdit_->setText(selectedItem_->parameterKey());
  parameterValueEdit_->setText(selectedItem_->parameterValue());
  parameterUnitEdit_->setText(selectedItem_->parameterUnit());
  requiredCheck_->setChecked(selectedItem_->required());

  updatingUi_ = false;

  updateParameterValidationUi();
}

void EditorScreenWidget::applyEditorToSelectedItem() {
  if (updatingUi_ || !selectedItem_) {
    return;
  }

  selectedItem_->setName(nameEdit_->text());
  selectedItem_->setType(configuredItemTypeFromString(typeCombo_->currentText()));
  selectedItem_->setDescription(descriptionEdit_->toPlainText());

  if (selectedItem_->isParameter()) {
    selectedItem_->setParameterKey(parameterKeyEdit_->text());
    selectedItem_->setParameterValue(parameterValueEdit_->text());
    selectedItem_->setParameterUnit(parameterUnitEdit_->text());
    selectedItem_->setRequired(requiredCheck_->isChecked());
  } else {
    selectedItem_->setParameterKey("");
    selectedItem_->setParameterValue("");
    selectedItem_->setParameterUnit("");
    selectedItem_->setRequired(false);
  }

  updateParameterValidationUi();

  parameterPanel_->setVisible(selectedItem_->isParameter());

  auto* currentTreeItem = tree_->currentItem();
  if (currentTreeItem) {
    currentTreeItem->setText(0, selectedItem_->name());
  }

  selectedItem_->setIsDirty(true);
  refreshTreeItemState(selectedItem_);
  emit projectModified(selectedItem_);
}

void EditorScreenWidget::setProjectName(const QString& name) {
  if (!project_) {
    return;
  }

  project_->setName(name);
}

ConfiguredProject* EditorScreenWidget::project() {
  return project_;
}

const ConfiguredProject* EditorScreenWidget::project() const {
  return project_;
}

void EditorScreenWidget::updateParameterValidationUi() {
  if (!selectedItem_) {
    setValidationState(nameEdit_, itemNameErrorLabel_, "");
    setValidationState(parameterKeyEdit_, parameterKeyErrorLabel_, "");
    setValidationState(parameterValueEdit_, parameterValueErrorLabel_, "");
    return;
  }

  ItemValidationContext context;
  context.item = selectedItem_;
  context.project = project_;

  ItemValidator validator;
  const ValidationResult result = validator.validate(context);

  selectedItem_->setHasError(!result.isValid());
  refreshTreeItemState(selectedItem_);

  const QString nameError = firstErrorForField(result, "name");
  setValidationState(nameEdit_, itemNameErrorLabel_, nameError);

  if (selectedItem_->isParameter()) {
    const QString keyError = firstErrorForField(result, "parameterKey");
    const QString valueError = firstErrorForField(result, "parameterValue");

    setValidationState(parameterKeyEdit_, parameterKeyErrorLabel_, keyError);
    setValidationState(parameterValueEdit_, parameterValueErrorLabel_, valueError);
  } else {
    setValidationState(parameterKeyEdit_, parameterKeyErrorLabel_, "");
    setValidationState(parameterValueEdit_, parameterValueErrorLabel_, "");
  }
}

void EditorScreenWidget::setProject(ConfiguredProject* project) {
  project_ = project;
  selectedItem_ = nullptr;
  itemToTreeItem_.clear();
  rebuildTree();
}

void EditorScreenWidget::setValidationState(QWidget* field, QLabel* errorLabel,
                                            const QString& errorText) {
  const bool valid = errorText.isEmpty();
  field->setStyleSheet(valid ? "" : "border: 2px solid #ff6b6b;");
  field->setToolTip(errorText);
  errorLabel->setText(errorText);
  errorLabel->setVisible(!valid);
}

void EditorScreenWidget::applyTreeItemState(QTreeWidgetItem* treeItem, const ConfiguredItem* item) {
  if (!treeItem || !item) {
    return;
  }

  switch (item->visualState()) {
    case ConfiguredItem::VisualState::Error:
      treeItem->setBackground(0, QBrush(QColor("#ff6b6b")));
      break;
    case ConfiguredItem::VisualState::Dirty:
      treeItem->setBackground(0, QBrush(QColor("#ffb347")));
      break;
    case ConfiguredItem::VisualState::Normal:
    default:
      treeItem->setBackground(0, QBrush(Qt::NoBrush));
      break;
  }
}

void EditorScreenWidget::refreshTreeItemState(ConfiguredItem* item) {
  if (!item) {
    return;
  }

  if (!item) {
    return;
  }

  auto it = itemToTreeItem_.find(item);
  if (it == itemToTreeItem_.end()) {
    return;
  }

  applyTreeItemState(it.value(), item);
}

void EditorScreenWidget::refreshTree() {
  rebuildTree();
}