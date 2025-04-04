#include "logger.h"

namespace fs = std::filesystem;

void Logger::print(std::string funcName, std::string message) {
  std::time_t now = std::time(nullptr);
  std::tm localTime;
  localtime_s(&localTime, &now);

  std::ostringstream timeStream;
  timeStream << "[" << std::put_time(&localTime, "%y.%m.%d %H:%M:%S") << "]";
  std::string timestamp = timeStream.str();

  std::ostringstream funcStream;
  funcStream << std::left << std::setw(FUNC_MAX_LEN) << funcName;

  std::ofstream logFile("latest.log", std::ios::app);
  if (logFile) {
    logFile << timestamp << " " << funcStream.str() << " : " << message
            << std::endl;
    logFile.close();

    spiltLogFile();
    compressOldLogFiles();
  }
}

void Logger::spiltLogFile() {
  fs::path fileName = "latest.log";

  if (!fs::exists(fileName)) return;

  auto fileSize = fs::file_size(fileName);

  if (fileSize <= LOG_MAX_SIZE) return;

  std::time_t now = std::time(nullptr);
  std::tm localTime;
  localtime_s(&localTime, &now);

  std::ostringstream oss;
  oss << "until_" << std::put_time(&localTime, "%y%m%d_%Hh_%Mm_%Ss") << ".log";
  fs::path newFileName = oss.str();

  fs::rename(fileName, newFileName);
}

void Logger::compressOldLogFiles() {
  std::vector<fs::directory_entry> logFiles;

  for (const auto& entry : fs::directory_iterator(".")) {
    if (entry.is_regular_file()) {
      std::string name = entry.path().filename().string();
      if (name.starts_with("until_") && name.ends_with(".log")) {
        logFiles.push_back(entry);
      }
    }
  }

  if (logFiles.size() <= 1) return;

  std::sort(logFiles.begin(), logFiles.end(),
            [](const fs::directory_entry& a, const fs::directory_entry& b) {
              return fs::last_write_time(a) > fs::last_write_time(b);
            });

  for (int i = 1; i < logFiles.size(); i++) {
    fs::path fileName = logFiles[i].path();
    fs::path newFileName = fileName;
    newFileName.replace_extension(".zip");

    fs::rename(fileName, newFileName);
  }
}