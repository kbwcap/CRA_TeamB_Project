#include "logger.h"

#include <filesystem>

void Logger::print(std::string funcName, std::string message) {
  {
    // 현재 날짜 및 시간 구하기
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);

    char timeBuffer[32];
    std::strftime(timeBuffer, sizeof(timeBuffer), "[%y.%m.%d %H:%M:%S]",
                  &localTime);

    std::ofstream logFile("latest.log", std::ios::app);
    if (logFile.is_open()) {
      logFile << timeBuffer << " " << funcName.append(30 - funcName.size(), ' ')
              << " : " << message << std::endl;
      logFile.close();
      
      manageLog();
    }
  }
}

void Logger::manageLog() {
  std::ifstream logFile("latest.log", std::ios::binary | std::ios::ate);
  if (!logFile.is_open()) {
    return;
  }

  // 파일 크기(바이트 단위) 가져오기
  std::streamsize fileSize = logFile.tellg();
  logFile.close();

  if (fileSize > 10 * 1024) {
    // 현재 날짜 및 시간 구하기
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);

    // 새로운 파일 이름 생성: until_yyMMdd_HHh_MMm_SSs.log
    char newFileName[64];
    std::strftime(newFileName, sizeof(newFileName),
                  "until_%y%m%d_%Hh_%Mm_%Ss.log", &localTime);

    if (std::rename("latest.log", newFileName) != 0) {
      std::perror("Error renaming log file");
    }
  }
}