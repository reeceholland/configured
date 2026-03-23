#pragma once

#include <QWidget>

class QTreeWidget;
class QTextEdit;

class EditorScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorScreenWidget(QWidget *parent = nullptr);

private:
    QTreeWidget *tree_ = nullptr;
    QTextEdit *properties_ = nullptr;

    void populateSampleTree();
};