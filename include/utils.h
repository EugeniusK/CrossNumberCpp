#ifndef UTILS_H
#define UTILS_H

#include <vector>

const int MAX_SQUARE_COUNT = 200;
const int MAX_ENTRY_VALUE = 10000;
const int MAX_HINT_COUNT = 100;

int get_nth_digit(int n, int digit, int total_digit);
int ipow(int base, int exp);
bool f(int a, int b, int c, int d);
int reverse_num(int n);
int dsum(int n);
void print_vec(std::vector<int> arr);

#endif