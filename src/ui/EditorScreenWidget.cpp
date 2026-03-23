#include "ui/EditorScreenWidget.hpp"

#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTextEdit>
#include <QSplitter>

EditorScreenWidget::EditorScreenWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QHBoxLayout(this);

    auto *splitter = new QSplitter(this);

    tree_ = new QTreeWidget(splitter);
    tree_->setHeaderLabel("Configuration");

    properties_ = new QTextEdit(splitter);
    properties_->setPlaceholderText("Select an item to edit properties...");

    splitter->addWidget(tree_);
    splitter->addWidget(properties_);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    populateSampleTree();

    connect(tree_, &QTreeWidget::itemClicked, this,
            [this](QTreeWidgetItem *item, int)
            {
                properties_->setText("Selected: " + item->text(0));
            });
}

void EditorScreenWidget::populateSampleTree()
{
    auto *robot = new QTreeWidgetItem(tree_);
    robot->setText(0, "Robot");

    auto *drivetrain = new QTreeWidgetItem(robot);
    drivetrain->setText(0, "Drivetrain");

    auto *localisation = new QTreeWidgetItem(robot);
    localisation->setText(0, "Localisation");

    auto *safety = new QTreeWidgetItem(robot);
    safety->setText(0, "Safety");

    tree_->expandAll();
}