#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdint>


#include "../lib/copy.h"

void PrintOutputFileErrorAndExit() {
  std::cerr << "Error! Output file wasn't found." << std::endl;
  std::exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
  const char* program_name = "";
  if (argc > 1) {
    program_name = argv[0];
  }

  if (argc < 3) {
    std::cerr << "Usage: ./labwork1 [OPTIONS] logs_filename" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  Arguments args = ParseArgs(argc, argv);  
  PrintArgs(&args);
  Logs* errors;
  if (args.kOutputFile) {
    errors = ReadLog(&args);
    WriteLogs(errors, &args);
    MessageSuccess(&args);
  }
  if (args.most_common) {
    if (!args.kOutputFile) {
      PrintOutputFileErrorAndExit();
    }
    MostCommon(errors, log_constants::kErrorsSize, args.most_common);
  }
  if (args.window_time) {
    ReadArray(&args);
  }

  if (args.kOutputFile) {
    for (std::size_t i = 0; i < log_constants::kErrorsSize; ++i) {
      delete[] errors[i].log_request;
      delete[] errors[i].log_status;
    }
    delete[] errors;
  }
  
  return 0;
}