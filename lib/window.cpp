#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "../lib/copy.h"

void SlidingWindow(Logs* logs_arr, Arguments* args, size_t i);

void ReadArray(Arguments* args) {
  if (args->window_time == 0) {

    return;
  }
  Logs* logs_arr = new Logs[log_constants::kLogsArraySize];
  const char* input_filename = nullptr;
  if (args->kLogFilename) {
    input_filename = args->kLogFilename;
  }
  std::FILE* input = std::fopen(input_filename, "r");
  if (!input) {
    PrintFileErrorAndExit(input_filename);
  }
  char buffer[log_constants::kBufferSize] { };

  size_t i = 0;
  while (true) {
    char* read = std::fgets(buffer, log_constants::kBufferSize, input);
    if (read == NULL) {
      break;
    }
    Logs data_stream = ParseLog(read, args);
    if (data_stream.log_time != 0) {
      logs_arr[i] = data_stream;
      ++i;
    }
  }

  SlidingWindow(logs_arr, args, i);

  fclose(input);
  delete[] logs_arr;
}

void SlidingWindow(Logs* logs_arr, Arguments* args, size_t i) {
  uint64_t array_size = i;
  int64_t cnt_request = 0;
  int64_t L = 0;
  int64_t max_number_of_requests = 0;
  int64_t ansL = 0;
  int64_t ansR = 0;

  for (int64_t R = 0; R < array_size; ++R) {
    ++cnt_request;
    while (R > L && logs_arr[R].log_time - logs_arr[L].log_time > args->window_time) {
      --cnt_request;
      ++L;
    }
    if ((cnt_request > max_number_of_requests) && (logs_arr[R].log_time - logs_arr[L].log_time > 0)) {
      max_number_of_requests = cnt_request;
      ansL = logs_arr[L].log_time;
      ansR = logs_arr[R].log_time;
    }
  }

  std::cout << "---| Window was calculated successfully |---" << std::endl;
  std::cout << "Window time = " << args->window_time << std::endl;
  std::cout << "Max quantity of requests = " << max_number_of_requests << std::endl;
  std::cout << "From " << ansL << " to " << ansR << " seconds" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}