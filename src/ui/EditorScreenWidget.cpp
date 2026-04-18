#include "ui/EditorScreenWidget.hpp"

#include <QCheckBox>
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

EditorScreenWidget::EditorScreenWidget(QWidget* parent)
    : QWidget(parent), project_(std::make_unique<ConfiguredProject>()) {
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
  createNewProject();
}

void EditorScreenWidget::createNewProject() {
  project_ = std::make_unique<ConfiguredProject>();
  project_->createSampleProject();
  currentFilePath_.clear();
  selectedItem_ = nullptr;
  rebuildTree();
}

bool EditorScreenWidget::saveProject(const QString& filePath) {
  if (!project_) {
    return false;
  }

  QString duplicate;
  if (project_->hasDuplicateParameterKeys(&duplicate)) {
    QMessageBox::warning(
        this, "Save Failed",
        QString("Cannot save project. Duplicate parameter key: '%1'").arg(duplicate));
    return false;
  }

  if (requiredCheck_->isChecked()
      && (parameterKeyEdit_->text().trimmed().isEmpty()
          || parameterValueEdit_->text().trimmed().isEmpty())) {
    QMessageBox::warning(this, "Save Failed",
                         "Cannot save project. Required parameters must have a key and value.");
    return false;
  }

  const bool ok = project_->saveToFile(filePath);
  if (ok) {
    currentFilePath_ = filePath;
  }

  return ok;
}

bool EditorScreenWidget::loadProject(const QString& filePath) {
  auto loadedProject = std::make_unique<ConfiguredProject>();
  if (!loadedProject->loadFromFile(filePath)) {
    return false;
  }

  project_ = std::move(loadedProject);
  currentFilePath_ = filePath;
  selectedItem_ = nullptr;
  rebuildTree();
  return true;
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
      childName = "New Subsystem";
      break;
    case ConfiguredItemType::Subsystem:
      childType = ConfiguredItemType::Component;
      childName = "New Component";
      break;
    case ConfiguredItemType::Component:
      childType = ConfiguredItemType::Parameter;
      childName = "New Parameter";
      break;
    case ConfiguredItemType::Parameter:
      childType = ConfiguredItemType::Parameter;
      childName = "New Parameter";
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
  selectedItem_ = nullptr;
  rebuildTree();
}

void EditorScreenWidget::rebuildTree() {
  tree_->clear();

  if (!project_ || !project_->root()) {
    return;
  }

  auto* rootItem = new QTreeWidgetItem(tree_);
  rootItem->setText(0, project_->root()->name());
  rootItem->setData(0, Qt::UserRole,
                    QVariant::fromValue(reinterpret_cast<quintptr>(project_->root())));

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
  typeCombo_->setCurrentText(typeToString(selectedItem_->type()));
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
  selectedItem_->setType(stringToType(typeCombo_->currentText()));
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
}

void EditorScreenWidget::setProjectName(const QString& name) {
  if (!project_) {
    return;
  }

  project_->setName(name);
}

QString EditorScreenWidget::typeToString(ConfiguredItemType type) const {
  switch (type) {
    case ConfiguredItemType::System:
      return "System";
    case ConfiguredItemType::Subsystem:
      return "Subsystem";
    case ConfiguredItemType::Component:
      return "Component";
    case ConfiguredItemType::Parameter:
      return "Parameter";
    default:
      return "Component";
  }
}

ConfiguredItemType EditorScreenWidget::stringToType(const QString& text) const {
  if (text == "System") {
    return ConfiguredItemType::System;
  }
  if (text == "Subsystem") {
    return ConfiguredItemType::Subsystem;
  }
  if (text == "Parameter") {
    return ConfiguredItemType::Parameter;
  }
  return ConfiguredItemType::Component;
}

ConfiguredProject* EditorScreenWidget::project() {
  return project_.get();
}

const ConfiguredProject* EditorScreenWidget::project() const {
  return project_.get();
}

QString EditorScreenWidget::currentFilePath() const {
  return currentFilePath_;
}

bool EditorScreenWidget::hasProjectFilePath() const {
  return !currentFilePath_.trimmed().isEmpty();
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
  context.project = project_.get();

  ItemValidator validator;
  const ValidationResult result = validator.validate(context);

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

void EditorScreenWidget::setProject(std::unique_ptr<ConfiguredProject> project,
                                    const QString& filePath) {
  project_ = std::move(project);
  currentFilePath_ = filePath;
  selectedItem_ = nullptr;
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
