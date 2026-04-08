#pragma once

#include <QString>
#include <memory>
#include <QSet>

class QJsonObject;
class ConfiguredItem;

class ConfiguredProject
{
public:
    ConfiguredProject();

    const QString &name() const;
    void setName(const QString &name);

    ConfiguredItem *root();
    const ConfiguredItem *root() const;

    void createSampleProject();

    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);

    bool hasDuplicateParameterKeys(QString *duplicateKey = nullptr) const;

private:
    QString name_;
    std::unique_ptr<ConfiguredItem> root_;

    QJsonObject itemToJson(const ConfiguredItem *item) const;
    std::unique_ptr<ConfiguredItem> itemFromJson(const QJsonObject &obj);
};