/*  SPMod - SourcePawn Scripting Engine for Half-Life
 *  Copyright (C) 2018  SPMod Development Team
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "LoggingSystem.hpp"

Logger::Logger(std::string_view prefix) : m_prefix(prefix)
{
}

ILogger *LoggerMngr::getLogger(const char *prefix)
{
    return getLoggerCore(prefix).get();
}

std::shared_ptr<Logger> LoggerMngr::getLoggerCore(std::string_view prefix)
{
    return m_loggers.try_emplace(prefix.data(), std::make_shared<Logger>(prefix)).first->second;
}

void Logger::setFilename(const char *filename)
{
    m_filename = filename;
}

void Logger::logToConsole(LogLevel level,
                          const char *format,
                          ...) const
{
    if (level < m_logLevel)
        return;

    char logMsg[512];
    va_list paramsList;

    va_start(paramsList, format);
    std::vsnprintf(logMsg, sizeof(logMsg), format, paramsList);
    va_end(paramsList);

    logToConsoleCore(level, logMsg);
}

void Logger::logToFile(LogLevel level,
                       const char *format,
                       ...) const
{
    if (level < m_logLevel)
        return;

    if (m_filename.empty())
        return;

    char logMsg[512];
    va_list paramsList;

    va_start(paramsList, format);
    std::vsnprintf(logMsg, sizeof(logMsg), format, paramsList);
    va_end(paramsList);

    logToFileCore(level, logMsg);
}

void Logger::logToBoth(LogLevel level, const char *format, ...) const
{
    if (level < m_logLevel)
        return;

    char logMsg[512];
    va_list paramsList;

    va_start(paramsList, format);
    std::vsnprintf(logMsg, sizeof(logMsg), format, paramsList);
    va_end(paramsList);

    logToBothCore(level, logMsg);
}

void Logger::sendMsgToConsole(const char *format, ...) const
{
    char logMsg[512];
    va_list paramsList;

    va_start(paramsList, format);
    std::vsnprintf(logMsg, sizeof(logMsg), format, paramsList);
    va_end(paramsList);

    sendMsgToConsoleCore(logMsg);
}

void Logger::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

LogLevel Logger::getLogLevel() const
{
    return m_logLevel;
}

void Logger::_writeToFile(std::string_view msg) const
{
    using fFlags = std::ios_base;

    time_t currentTime;
    time(&currentTime);
    tm convertedTime;

#if defined __STDC_LIB_EXT1__ || defined SP_MSVC
    #if defined SP_MSVC
    localtime_s(&convertedTime, &currentTime);
    #else
    localtime_s(&currentTime, &convertedTime);
    #endif
#else
    convertedTime = *std::localtime(&currentTime);
#endif

    char logDateTime[64], fileName[256];
    std::strftime(logDateTime, sizeof(logDateTime), "%Y/%m/%d - %H:%M:%S: ", &convertedTime);
    std::strftime(fileName, sizeof(fileName), "logs_%Y%m%d.log", &convertedTime);

    std::fstream logFile(gSPGlobal->getPathCore(DirType::Logs) / fileName, fFlags::out | fFlags::app | fFlags::ate);

    if (!logFile.tellg())
    {
        logFile << logDateTime << "Start of error session.\n";
        logFile << logDateTime << "Info (map ";

        if (STRING(gpGlobals->mapname))
            logFile << STRING(gpGlobals->mapname);
        else
            logFile << "<unknown>";

        logFile << ") (CRC ";

        if (gRehldsServerData)
            logFile << gRehldsServerData->GetWorldmapCrc();
        else
            logFile << "<unknown>";

        logFile << ")\n";
    }

    logFile << logDateTime << msg;
}
