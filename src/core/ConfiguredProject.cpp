#include "core/ConfiguredProject.hpp"
#include "core/ConfiguredItem.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

static bool collectParameterKeys(const ConfiguredItem *item,
                                 QSet<QString> &seen,
                                 QString *duplicate)
{
    if (!item)
    {
        return false;
    }

    if (item->isParameter())
    {
        const QString key = item->parameterKey().trimmed();

        if (!key.isEmpty())
        {
            if (seen.contains(key))
            {
                if (duplicate)
                {
                    *duplicate = key;
                }
                return true;
            }
            seen.insert(key);
        }
    }

    for (const auto &child : item->children())
    {
        if (collectParameterKeys(child.get(), seen, duplicate))
        {
            return true;
        }
    }

    return false;
}

ConfiguredProject::ConfiguredProject()
    : name_("Untitled Project"),
      description_(""),
      author_(""),
      company_(""),
      version_(""),
      robot_platform_(""),
      root_(std::make_unique<ConfiguredItem>("Robot", ConfiguredItemType::Robot))
{
}

const QString &ConfiguredProject::name() const
{
    return name_;
}

void ConfiguredProject::setName(const QString &name)
{
    name_ = name;
}

const QString &ConfiguredProject::description() const
{
    return description_;
}

void ConfiguredProject::setDescription(const QString &description)
{
    description_ = description;
}

const QString &ConfiguredProject::author() const
{
    return author_;
}

void ConfiguredProject::setAuthor(const QString &author)
{
    author_ = author;
}

const QString &ConfiguredProject::company() const
{
    return company_;
}

void ConfiguredProject::setCompany(const QString &company)
{
    company_ = company;
}

const QString &ConfiguredProject::version() const
{
    return version_;
}

void ConfiguredProject::setVersion(const QString &version)
{
    version_ = version;
}

const QString &ConfiguredProject::robotPlatform() const
{
    return robot_platform_;
}

void ConfiguredProject::setRobotPlatform(const QString &platform)
{
    robot_platform_ = platform;
}

bool ConfiguredProject::isGitManaged() const
{
    return git_managed_;
}

void ConfiguredProject::setGitManaged(bool managed)
{
    git_managed_ = managed;
}

ConfiguredItem *ConfiguredProject::root()
{
    return root_.get();
}

const ConfiguredItem *ConfiguredProject::root() const
{
    return root_.get();
}

void ConfiguredProject::createSampleProject()
{
    name_ = "Untitled Project";
    description_ = "";
    author_ = "";
    company_ = "";
    version_ = "";
    robot_platform_ = "";
    git_managed_ = false;

    root_ = std::make_unique<ConfiguredItem>("Robot", ConfiguredItemType::Robot);
    root_->setDescription("Top-level robot configuration.");

    auto drivetrain = std::make_unique<ConfiguredItem>("Drivetrain", ConfiguredItemType::Subsystem);
    drivetrain->setDescription("Drive motors and motion hardware.");

    auto leftMotor = std::make_unique<ConfiguredItem>("Left Motor", ConfiguredItemType::Component);
    leftMotor->setDescription("Left drive motor assembly.");

    auto maxRpm = std::make_unique<ConfiguredItem>("Max RPM", ConfiguredItemType::Parameter);
    maxRpm->setDescription("Maximum revolutions per minute.");
    maxRpm->setParameterKey("max_rpm");
    maxRpm->setParameterValue("150");
    maxRpm->setParameterUnit("rpm");
    maxRpm->setRequired(true);

    auto gearRatio = std::make_unique<ConfiguredItem>("Gear Ratio", ConfiguredItemType::Parameter);
    gearRatio->setDescription("Motor gearbox ratio.");
    gearRatio->setParameterKey("gear_ratio");
    gearRatio->setParameterValue("30");
    gearRatio->setParameterUnit(":1");
    gearRatio->setRequired(false);

    leftMotor->addChild(std::move(maxRpm));
    leftMotor->addChild(std::move(gearRatio));

    auto rightMotor = std::make_unique<ConfiguredItem>("Right Motor", ConfiguredItemType::Component);
    rightMotor->setDescription("Right drive motor assembly.");

    auto localisation = std::make_unique<ConfiguredItem>("Localisation", ConfiguredItemType::Subsystem);
    localisation->setDescription("Sensors and localisation sources.");

    auto imu = std::make_unique<ConfiguredItem>("IMU", ConfiguredItemType::Component);
    imu->setDescription("Inertial measurement unit.");

    auto imuFrame = std::make_unique<ConfiguredItem>("Frame ID", ConfiguredItemType::Parameter);
    imuFrame->setDescription("ROS frame ID for the IMU.");
    imuFrame->setParameterKey("frame_id");
    imuFrame->setParameterValue("imu_link");
    imuFrame->setParameterUnit("");
    imuFrame->setRequired(true);

    imu->addChild(std::move(imuFrame));

    localisation->addChild(std::move(imu));
    drivetrain->addChild(std::move(leftMotor));
    drivetrain->addChild(std::move(rightMotor));

    auto safety = std::make_unique<ConfiguredItem>("Safety", ConfiguredItemType::Subsystem);
    safety->setDescription("Safety-critical functions.");

    root_->addChild(std::move(drivetrain));
    root_->addChild(std::move(localisation));
    root_->addChild(std::move(safety));
}

bool ConfiguredProject::saveToFile(const QString &filePath) const
{
    if (!root_)
    {
        return false;
    }

    QJsonObject rootObj;
    rootObj["projectName"] = name_;
    rootObj["description"] = description_;
    rootObj["author"] = author_;
    rootObj["company"] = company_;
    rootObj["version"] = version_;
    rootObj["robotPlatform"] = robot_platform_;
    rootObj["gitManaged"] = git_managed_;
    rootObj["root"] = itemToJson(root_.get());

    QJsonDocument doc(rootObj);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

bool ConfiguredProject::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isObject())
    {
        return false;
    }

    const QJsonObject obj = doc.object();

    if (!obj.contains("projectName") || !obj.contains("root"))
    {
        return false;
    }

    name_ = obj["projectName"].toString("Untitled Project");
    description_ = obj["description"].toString();
    author_ = obj["author"].toString();
    company_ = obj["company"].toString();
    version_ = obj["version"].toString();
    robot_platform_ = obj["robotPlatform"].toString();
    git_managed_ = obj["gitManaged"].toBool();
    const QJsonObject rootObj = obj["root"].toObject();
    auto newRoot = itemFromJson(rootObj);
    if (!newRoot)
    {
        return false;
    }

    root_ = std::move(newRoot);
    return true;
}

QJsonObject ConfiguredProject::itemToJson(const ConfiguredItem *item) const
{
    QJsonObject obj;
    if (!item)
    {
        return obj;
    }

    QString typeString = "Component";
    switch (item->type())
    {
    case ConfiguredItemType::Robot:
        typeString = "Robot";
        break;
    case ConfiguredItemType::Subsystem:
        typeString = "Subsystem";
        break;
    case ConfiguredItemType::Component:
        typeString = "Component";
        break;
    case ConfiguredItemType::Parameter:
        typeString = "Parameter";
        break;
    }

    obj["name"] = item->name();
    obj["type"] = typeString;
    obj["description"] = item->description();
    obj["parameterKey"] = item->parameterKey();
    obj["parameterValue"] = item->parameterValue();
    obj["parameterUnit"] = item->parameterUnit();
    obj["required"] = item->required();

    QJsonArray childrenArray;
    for (const auto &child : item->children())
    {
        childrenArray.append(itemToJson(child.get()));
    }
    obj["children"] = childrenArray;

    return obj;
}

std::unique_ptr<ConfiguredItem> ConfiguredProject::itemFromJson(const QJsonObject &obj)
{
    const QString name = obj["name"].toString("Unnamed Item");
    const QString typeString = obj["type"].toString("Component");
    const QString description = obj["description"].toString();

    ConfiguredItemType type = ConfiguredItemType::Component;
    if (typeString == "Robot")
    {
        type = ConfiguredItemType::Robot;
    }
    else if (typeString == "Subsystem")
    {
        type = ConfiguredItemType::Subsystem;
    }
    else if (typeString == "Parameter")
    {
        type = ConfiguredItemType::Parameter;
    }

    auto item = std::make_unique<ConfiguredItem>(name, type);
    item->setDescription(description);
    item->setParameterKey(obj["parameterKey"].toString());
    item->setParameterValue(obj["parameterValue"].toString());
    item->setParameterUnit(obj["parameterUnit"].toString());
    item->setRequired(obj["required"].toBool(false));

    const QJsonArray childrenArray = obj["children"].toArray();
    for (const auto &childValue : childrenArray)
    {
        if (!childValue.isObject())
        {
            continue;
        }

        auto child = itemFromJson(childValue.toObject());
        if (child)
        {
            item->addChild(std::move(child));
        }
    }

    return item;
}

bool ConfiguredProject::hasDuplicateParameterKeys(QString *duplicateKey) const
{
    QSet<QString> seen;
    return collectParameterKeys(root_.get(), seen, duplicateKey);
}