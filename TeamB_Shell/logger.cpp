#include "logger.h"

void Logger::print(std::string funcName, std::string message) {
  {
    std::time_t now = std::time(nullptr);
    std::tm localTime;
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    char timeBuffer[32];
    std::strftime(timeBuffer, sizeof(timeBuffer), "[%y.%m.%d %H:%M:%S]",
                  &localTime);

    std::ofstream logFile("latest.log", std::ios::app);
    if (logFile.is_open()) {
      logFile << timeBuffer << " " << funcName.append(30 - funcName.size(), ' ')
              << " : " << message << std::endl;
    } else {
      std::cerr << "Error: Could not open log file!" << std::endl;
    }
  }
}