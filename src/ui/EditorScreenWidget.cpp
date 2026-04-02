#include "ui/EditorScreenWidget.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QVariant>

#include "core/ConfiguredProject.hpp"
#include "core/ConfiguredItem.hpp"

EditorScreenWidget::EditorScreenWidget(QWidget *parent)
    : QWidget(parent),
      project_(std::make_unique<ConfiguredProject>())
{
    auto *layout = new QHBoxLayout(this);

    auto *splitter = new QSplitter(this);

    tree_ = new QTreeWidget(splitter);
    tree_->setHeaderLabel("Configuration");

    auto *rightPanel = new QWidget(splitter);
    auto *rightLayout = new QVBoxLayout(rightPanel);

    auto *title = new QLabel("Properties", rightPanel);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto *form = new QFormLayout();

    nameEdit_ = new QLineEdit(rightPanel);

    typeCombo_ = new QComboBox(rightPanel);
    typeCombo_->addItems({"Robot", "Subsystem", "Component", "Parameter"});

    descriptionEdit_ = new QTextEdit(rightPanel);
    descriptionEdit_->setMinimumHeight(120);

    form->addRow("Name:", nameEdit_);
    form->addRow("Type:", typeCombo_);
    form->addRow("Description:", descriptionEdit_);

    parameterPanel_ = new QWidget(rightPanel);
    auto *parameterForm = new QFormLayout(parameterPanel_);

    parameterKeyEdit_ = new QLineEdit(parameterPanel_);
    parameterValueEdit_ = new QLineEdit(parameterPanel_);
    parameterUnitEdit_ = new QLineEdit(parameterPanel_);
    requiredCheck_ = new QCheckBox("Required", parameterPanel_);

    parameterForm->addRow("Parameter Key:", parameterKeyEdit_);
    parameterForm->addRow("Parameter Value:", parameterValueEdit_);
    parameterForm->addRow("Unit:", parameterUnitEdit_);
    parameterForm->addRow("", requiredCheck_);

    rightLayout->addWidget(title);
    rightLayout->addLayout(form);
    rightLayout->addWidget(parameterPanel_);
    rightLayout->addStretch();

    splitter->addWidget(tree_);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    connect(tree_, &QTreeWidget::currentItemChanged, this,
            [this](QTreeWidgetItem *current, QTreeWidgetItem *)
            {
                onTreeSelectionChanged(current);
            });

    connect(nameEdit_, &QLineEdit::textEdited, this,
            [this](const QString &)
            {
                applyEditorToSelectedItem();
            });

    connect(typeCombo_, &QComboBox::currentTextChanged, this,
            [this](const QString &)
            {
                applyEditorToSelectedItem();
            });

    connect(descriptionEdit_, &QTextEdit::textChanged, this,
            [this]()
            {
                applyEditorToSelectedItem();
            });

    connect(parameterKeyEdit_, &QLineEdit::textEdited, this,
            [this](const QString &)
            {
                applyEditorToSelectedItem();
            });

    connect(parameterValueEdit_, &QLineEdit::textEdited, this,
            [this](const QString &)
            {
                applyEditorToSelectedItem();
            });

    connect(parameterUnitEdit_, &QLineEdit::textEdited, this,
            [this](const QString &)
            {
                applyEditorToSelectedItem();
            });

    connect(requiredCheck_, &QCheckBox::checkStateChanged, this,
            [this](Qt::CheckState)
            {
                applyEditorToSelectedItem();
            });

    createNewProject();
}

void EditorScreenWidget::createNewProject()
{
    project_ = std::make_unique<ConfiguredProject>();
    project_->createSampleProject();
    selectedItem_ = nullptr;
    rebuildTree();
}

bool EditorScreenWidget::saveProject(const QString &filePath) const
{
    if (!project_)
    {
        return false;
    }

    return project_->saveToFile(filePath);
}

bool EditorScreenWidget::loadProject(const QString &filePath)
{
    auto loadedProject = std::make_unique<ConfiguredProject>();
    if (!loadedProject->loadFromFile(filePath))
    {
        return false;
    }

    project_ = std::move(loadedProject);
    selectedItem_ = nullptr;
    rebuildTree();
    return true;
}

void EditorScreenWidget::addChildToSelected()
{
    if (!selectedItem_)
    {
        return;
    }

    ConfiguredItemType childType = ConfiguredItemType::Component;
    QString childName = "New Item";

    switch (selectedItem_->type())
    {
    case ConfiguredItemType::Robot:
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

    if (childType == ConfiguredItemType::Parameter)
    {
        child->setParameterKey("new_key");
        child->setParameterValue("value");
        child->setParameterUnit("");
        child->setRequired(false);
    }

    selectedItem_->addChild(std::move(child));
    rebuildTree();
}

void EditorScreenWidget::removeSelectedItem()
{
    if (!selectedItem_)
    {
        return;
    }

    if (!project_ || selectedItem_ == project_->root())
    {
        return;
    }

    ConfiguredItem *parent = selectedItem_->parent();
    if (!parent)
    {
        return;
    }

    parent->removeChild(selectedItem_);
    selectedItem_ = nullptr;
    rebuildTree();
}

void EditorScreenWidget::rebuildTree()
{
    tree_->clear();

    if (!project_ || !project_->root())
    {
        return;
    }

    auto *rootItem = new QTreeWidgetItem(tree_);
    rootItem->setText(0, project_->root()->name());
    rootItem->setData(0, Qt::UserRole,
                      QVariant::fromValue(reinterpret_cast<quintptr>(project_->root())));

    for (auto &child : project_->root()->children())
    {
        addTreeItemRecursive(rootItem, child.get());
    }

    tree_->expandAll();
    tree_->setCurrentItem(rootItem);
}

void EditorScreenWidget::addTreeItemRecursive(QTreeWidgetItem *parentItem, ConfiguredItem *item)
{
    if (!parentItem || !item)
    {
        return;
    }

    auto *treeItem = new QTreeWidgetItem(parentItem);
    treeItem->setText(0, item->name());
    treeItem->setData(0, Qt::UserRole,
                      QVariant::fromValue(reinterpret_cast<quintptr>(item)));

    for (auto &child : item->children())
    {
        addTreeItemRecursive(treeItem, child.get());
    }
}

void EditorScreenWidget::onTreeSelectionChanged(QTreeWidgetItem *current)
{
    if (!current)
    {
        selectedItem_ = nullptr;
        loadSelectedItemIntoEditor();
        return;
    }

    selectedItem_ = reinterpret_cast<ConfiguredItem *>(
        current->data(0, Qt::UserRole).value<quintptr>());

    loadSelectedItemIntoEditor();
}

void EditorScreenWidget::loadSelectedItemIntoEditor()
{
    updatingUi_ = true;

    if (!selectedItem_)
    {
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
}

void EditorScreenWidget::applyEditorToSelectedItem()
{
    if (updatingUi_ || !selectedItem_)
    {
        return;
    }

    selectedItem_->setName(nameEdit_->text());
    selectedItem_->setType(stringToType(typeCombo_->currentText()));
    selectedItem_->setDescription(descriptionEdit_->toPlainText());

    if (selectedItem_->isParameter())
    {
        selectedItem_->setParameterKey(parameterKeyEdit_->text());
        selectedItem_->setParameterValue(parameterValueEdit_->text());
        selectedItem_->setParameterUnit(parameterUnitEdit_->text());
        selectedItem_->setRequired(requiredCheck_->isChecked());
    }
    else
    {
        selectedItem_->setParameterKey("");
        selectedItem_->setParameterValue("");
        selectedItem_->setParameterUnit("");
        selectedItem_->setRequired(false);
    }

    parameterPanel_->setVisible(selectedItem_->isParameter());

    auto *currentTreeItem = tree_->currentItem();
    if (currentTreeItem)
    {
        currentTreeItem->setText(0, selectedItem_->name());
    }
}

QString EditorScreenWidget::typeToString(ConfiguredItemType type) const
{
    switch (type)
    {
    case ConfiguredItemType::Robot:
        return "Robot";
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

ConfiguredItemType EditorScreenWidget::stringToType(const QString &text) const
{
    if (text == "Robot")
    {
        return ConfiguredItemType::Robot;
    }
    if (text == "Subsystem")
    {
        return ConfiguredItemType::Subsystem;
    }
    if (text == "Parameter")
    {
        return ConfiguredItemType::Parameter;
    }
    return ConfiguredItemType::Component;
}