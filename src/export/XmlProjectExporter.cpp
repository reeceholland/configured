#include "export/XmlProjectExporter.hpp"

#include <QSaveFile>
#include <QXmlStreamWriter>

#include "core/ConfiguredProject.hpp"
#include "export/ExportParameter.hpp"

bool XmlProjectExporter::exportParameters(const ConfiguredProject& project, const QString& filePath,
                                          QString* errorMessage) const {
  QSaveFile file(filePath);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    if (errorMessage) {
      *errorMessage = "Failed to open file: " + file.errorString();
    }
    return false;
  }

  QXmlStreamWriter xml(&file);
  xml.setAutoFormatting(true);

  xml.writeStartDocument();

  xml.writeStartElement("configured_project");

  xml.writeStartElement("metadata");
  xml.writeTextElement("project_name", project.name());
  xml.writeTextElement("last_modified", project.lastModified());
  xml.writeTextElement("git_manager", project.isGitManaged() ? "true" : "false");

  if (project.isGitManaged()) {
    if (!project.gitCommitHash().isEmpty()) {
      xml.writeTextElement("git_commit_hash", project.gitCommitHash());
    } else {
      xml.writeTextElement("git_commit_hash", "not available");
    }
  } else {
    xml.writeTextElement("version", project.version());
  }

  // Root container
  xml.writeStartElement("parameters");

  const auto params = project.collectParameters();

  for (const auto& param : params) {
    xml.writeStartElement("parameter");

    xml.writeAttribute("path", param.path);
    xml.writeAttribute("key", param.key);
    xml.writeAttribute("type", param.type);

    if (!param.unit.isEmpty()) {
      xml.writeAttribute("unit", param.unit);
    }

    xml.writeCharacters(param.value);

    xml.writeEndElement();  // parameter
  }

  xml.writeEndElement();  // parameters
  xml.writeEndElement();  // configured_project

  xml.writeEndDocument();

  if (!file.commit()) {
    if (errorMessage) {
      *errorMessage = "Failed to write file: " + file.errorString();
    }
    return false;
  }

  return true;
}