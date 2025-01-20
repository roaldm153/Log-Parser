#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "../lib/copy.h"

void WriteTimestamp(int64_t timestamp, FILE* output) {
  char str_timestamp[9];
  size_t i = 0;
  while (timestamp != 0) {
    int x = timestamp % 10;
    timestamp /= 10;
    char e = (char)(x + '0');
    str_timestamp[log_constants::kTimestampLen - i - 1] = e;
    ++i;
  }
  std::fwrite(str_timestamp, sizeof(char), log_constants::kTimestampLen, output);
}

void MessageSuccess(Arguments* args) {
  std::cout << "Logs with 5XX log status were written to " << args->kOutputFile << std::endl;
}

std::time_t ParseTime(char* log_time) {
  std::tm tm{};
  tm.tm_mday = (log_time[0] - '0') * 10 + (log_time[1] - '0');
  int time_zone_hours = (log_time[22] - '0') * 10 + (log_time[23] - '0');
  int local_time_zone_hours = 3;
  if (log_time[21] == '+') {
    time_zone_hours *= -1;
  }
  char month[4];
  month[3] = '\0';
  char year[5];
  year[4] = '\0';
  for (size_t i = 0; i < 3; ++i) {
    month[i] = log_time[i + 3];
  } 
  for (size_t i = 0; i < 4; ++i) {
    year[i] = log_time[i + 7];
  }
  tm.tm_year = (year[0] - '0') * 1000 + (year[1] - '0') * 100 +
               (year[2] - '0') * 10 + (year[3] - '0') - 1900;


  if (std::strcmp(month, "Jan") == 0) {
    tm.tm_mon = 0;
  }
  if (std::strcmp(month, "Feb") == 0) {
    tm.tm_mon = 1;
  }
  if (std::strcmp(month, "Mar") == 0) {
    tm.tm_mon = 2;
  }
  if (std::strcmp(month, "Apr") == 0) {
    tm.tm_mon = 3;
  }
  if (std::strcmp(month, "May") == 0) {
    tm.tm_mon = 4;
  }
  if (std::strcmp(month, "Jun") == 0) {
    tm.tm_mon = 5;
  }
  if (std::strcmp(month, "Jul") == 0) {
    tm.tm_mon = 6;
  }
  if (std::strcmp(month, "Aug") == 0) {
    tm.tm_mon = 7;
  }
  if (std::strcmp(month, "Sep") == 0) {
    tm.tm_mon = 8;
  }
  if (std::strcmp(month, "Oct") == 0) {
    tm.tm_mon = 9;
  }
  if (std::strcmp(month, "Nov") == 0) {
    tm.tm_mon = 10;
  }
  if (std::strcmp(month, "Dec") == 0) {
    tm.tm_mon = 11;
  }

  tm.tm_hour = ((log_time[12] - '0') * 10 + (log_time[13] - '0') + time_zone_hours - local_time_zone_hours) % 24;
  tm.tm_min = (log_time[15] - '0') * 10 + (log_time[16] - '0');
  tm.tm_sec = (log_time[18] - '0') * 10 + (log_time[19] - '0');

  std::time_t timestamp = mktime(&tm);

  return timestamp;
}

void TryParseSymbol(char* str, char**& split_str, char begin_symbol, char end_symbol, uint64_t& iterator) {
  while (*str != '\0') {
    if (*str == begin_symbol) {
      ++str;
      uint64_t back_pos = 0;
      while (*(str + back_pos) != end_symbol && *(str + back_pos) != '\0') {
        ++back_pos;
      }
      std::copy(str, str + back_pos, split_str[iterator]);
      ++iterator;
      str += back_pos + 1;
    } else {
      ++str;
    }
  }
}

void ParseStatus(char* str, char**& split_str, uint64_t& iterator) {
  const uint64_t kStatusLen = 3;
  size_t i = std::strlen(str) - 1;
  while (*(str + i) != ' ') {
    --i;
  }
  std::copy(str + i - kStatusLen, str + i, split_str[iterator]);
}

Logs ParseLog(char* str, Arguments* args) {
  char** split_str = new char* [log_constants::kSplitSize];
  for (size_t i = 0; i < log_constants::kSplitSize; ++i) {
    split_str[i] = new char[log_constants::kMaxLength] { };
    split_str[i][log_constants::kMaxLength - 1] = '\0';
  }
  while (*str == ' ') {
      ++str;
  }
  uint64_t iterator = 0;
  Logs data_stream;
  TryParseSymbol(str, split_str, '[', ']', iterator);
  TryParseSymbol(str, split_str, '\"', '\"', iterator);
  ParseStatus(str, split_str, iterator);
  int64_t log_time = ParseTime(split_str[0]);
  data_stream.log_time = log_time;
  if (split_str[2][0] == '5' && log_time >= args->from_time && log_time <= args->to_time) {
    data_stream.log_status = new char[std::strlen(split_str[2]) + 1];
    std::memcpy(data_stream.log_status, split_str[2], std::strlen(split_str[2]) + 1);
    data_stream.log_request = new char[std::strlen(split_str[1]) + 1];
    std::memcpy(data_stream.log_request, split_str[1], std::strlen(split_str[1]) + 1);
  }

  for(size_t i = 0; i < log_constants::kSplitSize; ++i) {
    delete[] split_str[i];
  } 
  delete[] split_str;

  return data_stream;
}

void PrintFileErrorAndExit(const char* filename) {
  std::cerr << "Error: file doesn't exist!" << std::endl;
  std::exit(EXIT_FAILURE);
}

Logs* ReadLog(Arguments* args) {
  const char* input_filename = nullptr;
  const char* output_filename = nullptr;
  if (args->kLogFilename) {
    input_filename = args->kLogFilename;
  }
  if (args->kOutputFile) {
    output_filename = args->kOutputFile;
  }
  std::FILE* input = std::fopen(input_filename, "r");
  std::FILE* output = std::fopen(output_filename, "w");
  if (!input) {
    PrintFileErrorAndExit(input_filename);
  }
  if (!output) {
    PrintFileErrorAndExit(output_filename);
  }
  char buffer[log_constants::kBufferSize] { };
  Logs* errors = new Logs[log_constants::kErrorsSize];
  size_t i = 0;
  while (true) {
    char* read = std::fgets(buffer, log_constants::kBufferSize, input);
    if (read == NULL) {
      break;
    }
    Logs data_stream = ParseLog(read, args);
    if (data_stream.log_request && data_stream.log_status && data_stream.log_time) {
      errors[i] = data_stream;
      ++i;
    }
  }
  fclose(input);
  if (output) {
    fclose(output);
  }
  return errors;
}

void PrintFileOpenErrorAndExit(const char* filename) {
  std::cerr << "Error! " << filename << " doesn't exist" << std::endl;
  std::exit(EXIT_FAILURE);
}

void PrintLogs(Logs* errors) {
  for (size_t i = 0; i < log_constants::kErrorsSize; ++i) {
      if (errors[i].log_status && errors[i].log_time && errors[i].log_request) {
        std::cout << errors[i].log_time << " " << errors[i].log_request << " " << errors[i].log_status << std::endl;
      }
  }
}

void WriteLogs(Logs* errors, Arguments* args) {
  if (args->need_print) {
    PrintLogs(errors);
  }
  const char* output_filename = args->kOutputFile;
  std::FILE* output = std::fopen(output_filename, "w");
  if (!output) {
    PrintFileOpenErrorAndExit(output_filename);
  }
  for(size_t i = 0; i < log_constants::kErrorsSize; ++i) {
    if (errors[i].log_time != 0 && errors[i].log_request != nullptr && errors[i].log_status != nullptr) {
      WriteTimestamp(errors[i].log_time, output);
      std::fwrite(" ", sizeof(char), 1, output);

      std::fwrite(errors[i].log_request, sizeof(char), std::strlen(errors[i].log_request), output);
      std::fwrite(" ", sizeof(char), 1, output);

      std::fwrite(errors[i].log_status, sizeof(char), std::strlen(errors[i].log_status), output);
      std::fwrite("\n", sizeof(char), 1, output);
    }
  }
}