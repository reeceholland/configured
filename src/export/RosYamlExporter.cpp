#include "export/RosYamlExporter.hpp"

#include <yaml-cpp/yaml.h>

#include <QRegularExpression>
#include <QSaveFile>
#include <QTextStream>

#include "core/ConfiguredProject.hpp"
#include "export/ExportParameter.hpp"

namespace {

QString rosNodeNameForProject(const ConfiguredProject& project) {
  QString name = project.name().trimmed();
  if (name.isEmpty()) {
    return "configured_node";
  }

  name.replace(QRegularExpression("[^A-Za-z0-9_]"), "_");
  name.replace(QRegularExpression("_+"), "_");
  name.remove(QRegularExpression("^_+|_+$"));

  if (name.isEmpty()) {
    return "configured_node";
  }

  if (name.front().isDigit()) {
    name.prepend("_");
  }

  return name;
}

bool isIntegerLiteral(const QString& value) {
  static const QRegularExpression integerPattern("^[+-]?\\d+$");
  return integerPattern.match(value).hasMatch();
}

bool isFloatingPointLiteral(const QString& value) {
  static const QRegularExpression floatingPointPattern(
      "^[+-]?(?:\\d+\\.\\d*|\\.\\d+|\\d+[eE][+-]?\\d+|\\d+\\.\\d*[eE][+-]?\\d+|\\.\\d+[eE][+-]?\\d+"
      ")$");
  return floatingPointPattern.match(value).hasMatch();
}

void writeTypedYamlValue(YAML::Emitter& yaml, const QString& rawValue) {
  const QString value = rawValue.trimmed();

  if (value.compare("true", Qt::CaseInsensitive) == 0) {
    yaml << true;
    return;
  }

  if (value.compare("false", Qt::CaseInsensitive) == 0) {
    yaml << false;
    return;
  }

  if (isIntegerLiteral(value)) {
    bool ok = false;
    const qlonglong integerValue = value.toLongLong(&ok);
    if (ok) {
      yaml << integerValue;
      return;
    }
  }

  if (isFloatingPointLiteral(value)) {
    bool ok = false;
    const double doubleValue = value.toDouble(&ok);
    if (ok) {
      yaml << doubleValue;
      return;
    }
  }

  yaml << YAML::DoubleQuoted << value.toStdString();
}

}  // namespace

bool RosYamlExporter::exportParameters(const ConfiguredProject& project, const QString& filePath,
                                       QString* errorMessage) const {
  QSaveFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    if (errorMessage) {
      *errorMessage = "Failed to open file: " + file.errorString();
    }
    return false;
  }

  YAML::Emitter yaml;
  yaml.SetIndent(2);
  yaml.SetDoublePrecision(6);
  yaml << YAML::BeginMap;
  yaml << YAML::Key << rosNodeNameForProject(project).toStdString();
  yaml << YAML::Value << YAML::BeginMap;
  yaml << YAML::Key << "ros__parameters";
  yaml << YAML::Value << YAML::BeginMap;

  const auto params = project.collectParameters();
  for (const auto& param : params) {
    const QString key = param.key.trimmed();
    if (key.isEmpty()) {
      continue;
    }

    yaml << YAML::Key << key.toStdString();
    yaml << YAML::Value;
    writeTypedYamlValue(yaml, param.value);
  }

  yaml << YAML::EndMap;
  yaml << YAML::EndMap;
  yaml << YAML::EndMap;

  if (!yaml.good()) {
    if (errorMessage) {
      *errorMessage = QString::fromStdString(yaml.GetLastError());
    }
    return false;
  }

  QTextStream out(&file);
  out << QString::fromStdString(yaml.c_str()) << "\n";

  if (!file.commit()) {
    if (errorMessage) {
      *errorMessage = "Failed to write file: " + file.errorString();
    }
    return false;
  }

  return true;
}
