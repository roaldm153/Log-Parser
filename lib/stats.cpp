#include <cstdio>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "../lib/copy.h"

struct Pair {
  size_t counter = 1;
  char* string = nullptr;
};

bool CompareString(char* a, char* b) {
  size_t i = 0;
  while (true) {
    if ((a[i] == '\0') || (b[i] == '\0')) {
      if (a[i] == '\0' && b[i] == '\0') {

        return true;
      }

      return false;
    }
    if (a[i] != b[i]) {

      return false;
    }

    ++i;
  }
}

void SwapPairs(Pair* a, Pair* b) {
  size_t tmp = a->counter;
  a->counter = b->counter;
  b->counter = tmp;
  char* tmp_str = a->string;
  a->string = b->string;
  b->string = tmp_str;
}

void BubbleSort(Pair* pairs, size_t n) {
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n - 1; ++j) {
      if (pairs[j].counter > pairs[j + 1].counter) {
        SwapPairs(&pairs[j], &pairs[j + 1]);
      }
    }
  }
}

void MostCommon(Logs* errors, uint64_t n, uint64_t most_common) {
  Pair pairs[n];
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < n; ++j) {
      if (i != j && errors[i].log_request != nullptr && errors[j].log_request != nullptr &&
         CompareString(errors[i].log_request, errors[j].log_request)) {
        if (pairs[j].string == nullptr) {
          pairs[j].string = errors[j].log_request;
        }
        ++pairs[j].counter;
      }
    }
  }

  BubbleSort(pairs, n);
  std::cout << "---| MOST COMMON REQUESTS |---" << std::endl;
  char* last_string = nullptr;

  for (size_t i = n - 1; i > 0; --i) {
    if (most_common > 0 && pairs[i].string != nullptr) {
      if (last_string != nullptr &&
          !CompareString(last_string, pairs[i].string)) {
        std::cout << pairs[i].counter << " " << pairs[i].string << std::endl;
        last_string = pairs[i].string;
        --most_common;
      }
      if (last_string == nullptr) {
        last_string = pairs[i].string;
        --most_common;
        std::cout << pairs[i].counter << " " << pairs[i].string << std::endl;
      }
    }
    if (most_common <= 0) {
      break;
    }
  }
  std::cout << "----------------------------------------" << std::endl;
}