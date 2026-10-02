#include "utils.h"

#include <cmath>
#include <iostream>

int get_nth_digit(int n, int digit, int total_digit) {
  return (n / ipow(10, total_digit - digit - 1)) % 10;
}
int ipow(int base, int exp) {
  int result = 1;
  for (int i = 0; i < exp; ++i) {
    result *= base;
  }
  return result;
}

bool f(int a, int b, int c, int d) {
  if (b == c) {
    return false;
  }
  return true;
};

int dsum(int n) {
  int sum = 0;
  int num = std::abs(n);
  while (num != 0) {
    int last = num % 10;
    sum += last;
    num /= 10;
  }
  return sum;
}

int reverse_num(int n) {
  int rev_num = 0;
  int num = std::abs(n);
  while (num > 0) {
    rev_num = rev_num * 10 + num % 10;
    num = num / 10;
  }
  return rev_num;
}

void print_vec(std::vector<int> arr) {
  for (int i : arr) {
    std::cout << i << ",";
  }
  std::cout << std::endl;
};