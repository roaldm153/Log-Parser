#include <iostream>
#include <cstring>
#include <fstream>
#include <limits>
#include <cinttypes>
#include <charconv>
#include <system_error>

#include "../lib/copy.h"

namespace args_keys {
  const char* kStatsFullKey = "--stats=";
  const char* kStatsShortKey = "-s";
  const uint8_t kStatsFullKeyLen = 8;
  const uint8_t kStatsShortKeyLen = 2;

  const char* kOutputFullKey = "--output=";
  const char* kOutputShortKey = "-o";
  const uint8_t kOutputFullKeyLen = 9;
  const uint8_t kOutputShortKeyLen = 2;

  const char* kWindowFullKey = "--window=";
  const char* kWindowShortKey = "-w";
  const uint8_t kWindowFullKeyLen = 9;
  const uint8_t kWindowShortKeyLen = 2;

  const char* kPrintFullKey = "--print";
  const char* kPrintShortKey = "-p";
  const uint8_t kPrintFullKeyLen = 7;
  const uint8_t kPrintShortKeyLen = 2;

  const char* kToTimeFullKey = "--to=";
  const char* kToTimeShortKey = "-e";
  const uint8_t kToTimeFullKeyLen = 5;
  const uint8_t kToTimeShortKeyLen = 2;

  const char* kFromTimeFullKey = "--from=";
  const char* kFromTimeShortKey = "-f";
  const uint8_t kFromTimeFullKeyLen = 7;
  const uint8_t kFromTimeShortKeyLen = 2;
}

void PrintArgs(Arguments* args) {
  std::cout << "----------| LogAnalyzer v1.0 |----------" << std::endl;
  if (args->need_print == true) {
    std::cout << "PRINT: " << "true" << std::endl;
  } else {
    std::cout << "PRINT: " << "false" << std::endl;
  }
  if (args->kProgramPath) {
    std::cout <<"PATH: " << args->kProgramPath << std::endl;
  }
  if (args->kLogFilename) {
    std::cout << "LOG FILE: " << args->kLogFilename << std::endl;
  }
  if (args->kOutputFile) {
    std::cout << "OUTPUT FILE: " << args->kOutputFile << std::endl;
  }
  if (args->window_time) {
    std::cout << "WINDOW TIME: " << args->window_time << std::endl;
  }
  if (args->most_common) {
    std::cout << "STATS: " << args->most_common << std::endl;
  }
  if (args->from_time) {
    std::cout << "FROM: " << args->from_time << std::endl;
  }
  if (args->to_time != UINT64_MAX) {
    std::cout << "TO: " << args->to_time << std::endl;
  }
  
  std::cout << "----------------------------------------" << std::endl;
}

void PrintErrorAndExit(const char* kFullKey, const char* kShortKey) {
  std::cerr << "Error! Usage " << kFullKey << " or " << kShortKey << "." << std::endl;
  std::exit(EXIT_FAILURE);
}

bool TryParseUInt64T(char* str, uint64_t& dest) {
  char* str_end = str + std::strlen(str);
  auto parse_result = std::from_chars(str, str_end, dest);

  if (parse_result.ec == std::errc::invalid_argument) {
    std::cerr << "Invalid argument: " << str << " isn't a number." << std::endl;

    return false;
  }
  if (parse_result.ec == std::errc::result_out_of_range) {
    std::cerr << "Invalid argument: " << str << " out of range." << std::endl;

    return false;
  }
  if (parse_result.ec == std::errc()) {
    if (parse_result.ptr != str_end) {
      std::cerr << "Invalid argument: " << str << " isn't a number." << std::endl;

      return false;
    }

    return true;
  }

  return false;
}

Arguments ParseArgs(int argc, char** argv) {

  Arguments args;
  args.kProgramPath = argv[0];

  for (size_t i = 1; i < argc; ++i) {
    
    if (strncmp(argv[i], args_keys::kStatsFullKey, args_keys::kStatsFullKeyLen) == 0) {
      uint64_t tmp_dest;
      if (!TryParseUInt64T((argv[i] + args_keys::kStatsFullKeyLen), tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.most_common = tmp_dest;
    } else if (strncmp(argv[i], args_keys::kStatsFullKey, args_keys::kStatsFullKeyLen - 1) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kStatsFullKey, args_keys::kStatsShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.most_common = tmp_dest;

    } else if (strncmp(argv[i], args_keys::kStatsShortKey, args_keys::kStatsShortKeyLen) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kStatsFullKey, args_keys::kStatsShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.most_common = tmp_dest;
    }  
    
    if (strncmp(argv[i], args_keys::kWindowFullKey, args_keys::kWindowFullKeyLen) == 0) {
      uint64_t tmp_dest;
      if (!TryParseUInt64T(argv[i] + args_keys::kWindowFullKeyLen, tmp_dest)) {
        std::exit(EXIT_FAILURE);
      } 
      args.window_time = tmp_dest;

    } else if (strncmp(argv[i], args_keys::kWindowFullKey, args_keys::kWindowFullKeyLen - 1) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kWindowFullKey, args_keys::kWindowShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.window_time = tmp_dest;

    } else if (strncmp(argv[i], args_keys::kWindowShortKey, args_keys::kWindowShortKeyLen) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kWindowFullKey, args_keys::kWindowShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.window_time = tmp_dest;
    }

    if (strncmp(argv[i], args_keys::kOutputFullKey, args_keys::kOutputFullKeyLen) == 0) { // CHECK OUTPUT
      args.kOutputFile = (argv[i] + args_keys::kOutputFullKeyLen);

    } else if (strncmp(argv[i], args_keys::kOutputFullKey, args_keys::kOutputFullKeyLen - 1) == 0) {
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kOutputFullKey, args_keys::kOutputShortKey);
      }
      args.kOutputFile = argv[i + 1];

    } else if (strncmp(argv[i], args_keys::kOutputShortKey, args_keys::kOutputShortKeyLen) == 0) {
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kOutputFullKey, args_keys::kOutputShortKey);
      }
      args.kOutputFile = argv[i + 1];

    }
    
    if (strncmp(argv[i], args_keys::kPrintFullKey, args_keys::kPrintFullKeyLen) == 0) {
      args.need_print = true;
    }
    if (strncmp(argv[i], args_keys::kPrintShortKey, args_keys::kPrintShortKeyLen) == 0) {
      args.need_print = true;
    }

    if (strncmp(argv[i], args_keys::kFromTimeFullKey, args_keys::kFromTimeFullKeyLen) == 0) {
      uint64_t tmp_dest;
      if (!TryParseUInt64T(argv[i] + args_keys::kFromTimeFullKeyLen, tmp_dest)) {
        std::exit(EXIT_FAILURE);
      } 
      args.from_time = tmp_dest;

    } else if (strncmp(argv[i], args_keys::kFromTimeShortKey, args_keys::kFromTimeShortKeyLen) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kFromTimeFullKey, args_keys::kFromTimeShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.from_time = tmp_dest;
    }

    if (strncmp(argv[i], args_keys::kToTimeFullKey, args_keys::kToTimeFullKeyLen) == 0) {
      uint64_t tmp_dest;
      if (!TryParseUInt64T(argv[i] + args_keys::kToTimeFullKeyLen, tmp_dest)) {
        std::exit(EXIT_FAILURE);
      } 
      args.to_time = tmp_dest;

    } else if (strncmp(argv[i], args_keys::kToTimeShortKey, args_keys::kToTimeShortKeyLen) == 0) {
      uint64_t tmp_dest;
      if (i + 1 >= argc) {
        PrintErrorAndExit(args_keys::kToTimeFullKey, args_keys::kToTimeShortKey);
      }
      if (!TryParseUInt64T(argv[i + 1], tmp_dest)) {
        std::exit(EXIT_FAILURE);
      }
      args.to_time = tmp_dest;
    }

    if (i > 0 && (argv[i][0] != '-')) {
      if (i - 1 == 0) {
        args.kLogFilename = argv[i];
      }
      if (i - 1 > 0) {
        if (argv[i - 1][0] != '-') {
          args.kLogFilename = argv[i];
        }
        if (argv[i - 1][0] == '-' && argv[i - 1][1] == '-') {
          args.kLogFilename = argv[i];
        }
      }
    }
  }

  return args;
}