#include "core/ConfiguredProject.hpp"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

#include "core/ConfiguredItem.hpp"

static bool collectParameterKeys(const ConfiguredItem* item, QSet<QString>& seen,
                                 QString* duplicate) {
  if (!item) {
    return false;
  }

  if (item->isParameter()) {
    const QString key = item->parameterKey().trimmed();

    if (!key.isEmpty()) {
      if (seen.contains(key)) {
        if (duplicate) {
          *duplicate = key;
        }
        return true;
      }
      seen.insert(key);
    }
  }

  for (const auto& child : item->children()) {
    if (collectParameterKeys(child.get(), seen, duplicate)) {
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
      git_managed_(false),
      gitCommitHash_(""),
      last_modified_(QDateTime::currentDateTime().toString(Qt::ISODate)),
      root_(std::make_unique<ConfiguredItem>("System", ConfiguredItemType::System)) {}

const QString& ConfiguredProject::name() const {
  return name_;
}

void ConfiguredProject::setName(const QString& name) {
  name_ = name;
}

const QString& ConfiguredProject::description() const {
  return description_;
}

void ConfiguredProject::setDescription(const QString& description) {
  description_ = description;
}

const QString& ConfiguredProject::author() const {
  return author_;
}

void ConfiguredProject::setAuthor(const QString& author) {
  author_ = author;
}

const QString& ConfiguredProject::company() const {
  return company_;
}

void ConfiguredProject::setCompany(const QString& company) {
  company_ = company;
}

const QString& ConfiguredProject::version() const {
  return version_;
}

void ConfiguredProject::setVersion(const QString& version) {
  version_ = version;
}

const QString& ConfiguredProject::lastModified() const {
  return last_modified_;
}

void ConfiguredProject::setLastModified(const QString& lastModified) {
  last_modified_ = lastModified;
}

const QString& ConfiguredProject::robotPlatform() const {
  return robot_platform_;
}

void ConfiguredProject::setRobotPlatform(const QString& platform) {
  robot_platform_ = platform;
}

bool ConfiguredProject::isGitManaged() const {
  return git_managed_;
}

void ConfiguredProject::setGitManaged(bool managed) {
  git_managed_ = managed;
}

const QString& ConfiguredProject::gitCommitHash() const {
  return gitCommitHash_;
}

void ConfiguredProject::setGitCommitHash(const QString& hash) {
  gitCommitHash_ = hash;
}

ConfiguredItem* ConfiguredProject::root() {
  return root_.get();
}

const ConfiguredItem* ConfiguredProject::root() const {
  return root_.get();
}

void ConfiguredProject::createSampleProject() {
  name_ = "Untitled Project";
  description_ = "";
  author_ = "";
  company_ = "";
  version_ = "";
  robot_platform_ = "";
  last_modified_ = QDateTime::currentDateTime().toString(Qt::ISODate);
  git_managed_ = false;

  root_ = std::make_unique<ConfiguredItem>("System", ConfiguredItemType::System);
  root_->setDescription("Top-level system configuration.");
}

bool ConfiguredProject::saveToFile(const QString& filePath, QString* error) {
  if (!root_) {
    if (error) {
      *error = "Project has no root item.";
    }
    return false;
  }

  QJsonObject rootObj;
  rootObj["projectName"] = name_;
  rootObj["description"] = description_;
  rootObj["author"] = author_;
  rootObj["company"] = company_;
  rootObj["version"] = version_;

  setLastModified(QDateTime::currentDateTime().toString(Qt::ISODate));

  rootObj["lastModified"] = last_modified_;
  rootObj["robotPlatform"] = robot_platform_;
  rootObj["gitManaged"] = git_managed_;
  rootObj["gitCommitHash"] = gitCommitHash_;
  rootObj["root"] = itemToJson(root_.get());

  QJsonDocument doc(rootObj);

  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly)) {
    if (error) {
      *error = file.errorString();
    }
    return false;
  }

  file.write(doc.toJson(QJsonDocument::Indented));
  return true;
}

bool ConfiguredProject::loadFromFile(const QString& filePath, QString* error) {
  QFile file(filePath);
  if (!file.open(QIODevice::ReadOnly)) {
    if (error) {
      *error = file.errorString();
    }
    return false;
  }

  const QByteArray data = file.readAll();
  const QJsonDocument doc = QJsonDocument::fromJson(data);

  if (!doc.isObject()) {
    if (error) {
      *error = "Invalid JSON format.";
    }
    return false;
  }

  const QJsonObject obj = doc.object();

  if (!obj.contains("projectName") || !obj.contains("root")) {
    return false;
  }

  name_ = obj["projectName"].toString("Untitled Project");
  description_ = obj["description"].toString();
  author_ = obj["author"].toString();
  company_ = obj["company"].toString();
  version_ = obj["version"].toString();
  last_modified_ = obj["lastModified"].toString();
  robot_platform_ = obj["robotPlatform"].toString();
  git_managed_ = obj["gitManaged"].toBool();
  const QJsonObject rootObj = obj["root"].toObject();
  auto newRoot = itemFromJson(rootObj);
  if (!newRoot) {
    return false;
  }

  root_ = std::move(newRoot);
  return true;
}

QJsonObject ConfiguredProject::itemToJson(const ConfiguredItem* item) const {
  QJsonObject obj;
  if (!item) {
    return obj;
  }

  QString typeString = "Component";
  switch (item->type()) {
    case ConfiguredItemType::System:
      typeString = "System";
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
  for (const auto& child : item->children()) {
    childrenArray.append(itemToJson(child.get()));
  }
  obj["children"] = childrenArray;

  return obj;
}

std::unique_ptr<ConfiguredItem> ConfiguredProject::itemFromJson(const QJsonObject& obj) {
  const QString name = obj["name"].toString("Unnamed Item");
  const QString typeString = obj["type"].toString("Component");
  const QString description = obj["description"].toString();

  ConfiguredItemType type = ConfiguredItemType::Component;
  if (typeString == "System") {
    type = ConfiguredItemType::System;
  } else if (typeString == "Subsystem") {
    type = ConfiguredItemType::Subsystem;
  } else if (typeString == "Parameter") {
    type = ConfiguredItemType::Parameter;
  }

  auto item = std::make_unique<ConfiguredItem>(name, type);
  item->setDescription(description);
  item->setParameterKey(obj["parameterKey"].toString());
  item->setParameterValue(obj["parameterValue"].toString());
  item->setParameterUnit(obj["parameterUnit"].toString());
  item->setRequired(obj["required"].toBool(false));

  const QJsonArray childrenArray = obj["children"].toArray();
  for (const auto& childValue : childrenArray) {
    if (!childValue.isObject()) {
      continue;
    }

    auto child = itemFromJson(childValue.toObject());
    if (child) {
      item->addChild(std::move(child));
    }
  }

  return item;
}

bool ConfiguredProject::hasDuplicateParameterKeys(QString* duplicateKey) const {
  QSet<QString> seen;
  return collectParameterKeys(root_.get(), seen, duplicateKey);
}

std::vector<ExportParameter> ConfiguredProject::collectParameters() const {
  std::vector<ExportParameter> result;

  if (!root_) {
    return result;
  }

  collectParametersRecursive(root_.get(), QString(), result);
  return result;
}

void ConfiguredProject::collectParametersRecursive(const ConfiguredItem* item,
                                                   const QString& parentPath,
                                                   std::vector<ExportParameter>& out) const {
  if (!item) {
    return;
  }

  const QString currentPath = parentPath.isEmpty() ? item->name() : parentPath + "." + item->name();

  if (item->isParameter()) {
    ExportParameter param;
    param.path = currentPath;
    param.key = item->keyAsString();
    param.type = item->valueTypeString();
    param.value = item->valueAsString();
    param.unit = item->unitAsString();
    out.push_back(param);
  }

  for (const auto& child : item->children()) {
    collectParametersRecursive(child.get(), currentPath, out);
  }
}