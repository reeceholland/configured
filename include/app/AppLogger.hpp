/**
 * @file AppLogger.hpp
 * @author Reece Holland
 * @brief Logging utility for the Configured application.
 * @version 0.1
 * @date 2026-04-21
 *
 * @copyright Copyright (c) 2026 Reece Holland
 * License: MIT License (https://opensource.org/licenses/MIT)
 *
 */

#pragma once

#include <QString>

/**
 * @brief Logging utility for the Configured application.
 *
 * The AppLogger class provides static methods for installing a logging mechanism that captures
 * log messages and writes them to a file. This allows for easier debugging and issue tracking by
 * maintaining a record of application events and errors. The log file path can be retrieved for
 * reference or display purposes.
 */
class AppLogger {
 public:
  /**
   * @brief Installs the logging mechanism for the application.
   *
   * This method sets up the logging system to capture log messages and write them to a file.
   * If an error occurs during installation, it can be retrieved through the `error` parameter.
   *
   * @param error Pointer to a QString to receive error messages, if any.
   * @return true if the logging mechanism was successfully installed.
   * @return false if an error occurred during installation.
   */
  static bool install(QString* error = nullptr);

  /**
   * @brief Retrieves the path to the log file.
   *
   * This method returns the file path where log messages are being written.
   *
   * @return QString The path to the log file.
   */
  static QString logFilePath();
};
