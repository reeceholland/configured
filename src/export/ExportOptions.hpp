#pragma once

#include <QString>

#include "export/ExportFormat.hpp"

struct ExportOptions
{
    ExportFormat format;
    QString outputPath;
};