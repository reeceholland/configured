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

    const QString &description() const;
    void setDescription(const QString &description);

    const QString &author() const;
    void setAuthor(const QString &author);

    const QString &company() const;
    void setCompany(const QString &company);

    const QString &version() const;
    void setVersion(const QString &version);

    const QString &lastModified() const;
    void setLastModified(const QString &lastModified);

    const QString &robotPlatform() const;
    void setRobotPlatform(const QString &platform);

    bool isGitManaged() const;
    void setGitManaged(bool managed);

    ConfiguredItem *root();
    const ConfiguredItem *root() const;

    void createSampleProject();

    bool saveToFile(const QString &filePath) const;
    bool loadFromFile(const QString &filePath);

    bool hasDuplicateParameterKeys(QString *duplicateKey = nullptr) const;

private:
    QString name_;
    QString description_;
    QString author_;
    QString company_;
    QString version_;
    QString last_modified_;
    QString robot_platform_;
    bool git_managed_ = false;
    std::unique_ptr<ConfiguredItem> root_;

    QJsonObject itemToJson(const ConfiguredItem *item) const;
    std::unique_ptr<ConfiguredItem> itemFromJson(const QJsonObject &obj);
};