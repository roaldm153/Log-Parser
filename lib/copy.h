#pragma once

#include <iostream>
#include <cstdint>

struct Arguments {
  const char* kProgramPath = nullptr;
  const char* kLogFilename = nullptr;
  const char* kOutputFile = nullptr;

  uint64_t window_time = 0;
  uint64_t from_time = 0;
  uint64_t to_time = UINT64_MAX;
  uint64_t error_counter = 0;
  uint64_t most_common = 0;

  bool need_print = false;
};

struct Logs {
  int64_t log_time = 0;
  char* log_request = nullptr;
  char* log_status = nullptr;
};

namespace log_constants {
  const uint64_t kBufferSize = 1024;
  const uint64_t kMaxLength = 512;
  const uint64_t kSplitSize = 9;
  const uint64_t kLogTimeSize = 21;
  const uint64_t kLogRequestSize = 512;
  const uint64_t kLogStatusSize = 4;
  const uint64_t kTimestampLen = 9;
  const uint64_t kLogsArraySize = 2e6;
  const uint64_t kErrorsSize = 100;
}

Arguments ParseArgs(int argc, char** argv);
bool TryParseUInt64T(char* str, uint64_t& dest);
void MostCommon(Logs* errors, uint64_t n, uint64_t most_common);
void PrintArgs(Arguments* args);
void WriteLogs(Logs* errors, Arguments* args);
void PrintFileErrorAndExit(const char* filename);
void ReadArray(Arguments* args);
void MessageSuccess(Arguments* args);
void PrintLogs(Logs* errors);
Logs ParseLog(char* str, Arguments* args);
Logs* ReadLog(Arguments* args);