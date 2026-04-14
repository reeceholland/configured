#include "export/JsonProjectExporter.hpp"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

#include "core/ConfiguredProject.hpp"
#include "export/ExportParameter.hpp"

bool JsonProjectExporter::exportParameters(const ConfiguredProject& project,
                                           const QString& filePath, QString* errorMessage) const {
  const auto params = project.collectParameters();

  QJsonArray paramArray;

  for (const auto& param : params) {
    QJsonObject obj;
    obj["path"] = param.path;
    obj["key"] = param.key;
    obj["type"] = param.type;
    obj["value"] = param.value;

    if (!param.unit.isEmpty()) {
      obj["unit"] = param.unit;
    }

    paramArray.append(obj);
  }

  QJsonObject metadata;
  metadata["projectName"] = project.name();
  metadata["lastModified"] = project.lastModified();
  if (project.isGitManaged()) {
    metadata["gitCommitHash"] = project.gitCommitHash();
  } else {
    metadata["version"] = project.version();
  }

  QJsonObject root;
  root["metadata"] = metadata;
  root["parameters"] = paramArray;

  QJsonDocument doc(root);

  QSaveFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    if (errorMessage) {
      *errorMessage = "Failed to open file: " + file.errorString();
    }
    return false;
  }

  file.write(doc.toJson(QJsonDocument::Indented));

  if (!file.commit()) {
    if (errorMessage) {
      *errorMessage = "Failed to write file: " + file.errorString();
    }
    return false;
  }

  return true;
}