#pragma once

#include <QWidget>
#include <memory>

#include "core/ConfiguredItem.hpp"

class QTreeWidget;
class QTreeWidgetItem;
class QLineEdit;
class QComboBox;
class QTextEdit;
class QCheckBox;
class QWidget;
class ConfiguredProject;

class EditorScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorScreenWidget(QWidget *parent = nullptr);

    void createNewProject();
    void addChildToSelected();
    void removeSelectedItem();

    void setProjectName(const QString &name);
    bool saveProject(const QString &filePath);
    bool loadProject(const QString &filePath);

    QString currentFilePath() const;
    bool hasProjectFilePath() const;

    ConfiguredProject *project();
    const ConfiguredProject *project() const;

private:
    QTreeWidget *tree_ = nullptr;

    QLineEdit *nameEdit_ = nullptr;
    QComboBox *typeCombo_ = nullptr;
    QTextEdit *descriptionEdit_ = nullptr;

    QWidget *parameterPanel_ = nullptr;
    QLineEdit *parameterKeyEdit_ = nullptr;
    QLineEdit *parameterValueEdit_ = nullptr;
    QLineEdit *parameterUnitEdit_ = nullptr;
    QCheckBox *requiredCheck_ = nullptr;

    QString currentFilePath_;

    std::unique_ptr<ConfiguredProject> project_;
    ConfiguredItem *selectedItem_ = nullptr;
    bool updatingUi_ = false;

    void rebuildTree();
    void addTreeItemRecursive(QTreeWidgetItem *parentItem, ConfiguredItem *item);

    void onTreeSelectionChanged(QTreeWidgetItem *current);
    void loadSelectedItemIntoEditor();
    void applyEditorToSelectedItem();

    QString typeToString(ConfiguredItemType type) const;
    ConfiguredItemType stringToType(const QString &text) const;
};