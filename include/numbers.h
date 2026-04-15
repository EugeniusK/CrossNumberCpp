#ifndef NUMBERS_H
#define NUMBERS_H

#include <algorithm>
#include <vector>
std::vector<int> multiple_3(int n);
std::vector<int> multiple_7(int n);
std::vector<int> multiple_1102(int n);
std::vector<int> multiple_2020(int n);
std::vector<int> multiple_11111(int n);
std::vector<int> dsum_9(int n);
std::vector<int> dsum_13(int n);
std::vector<int> dsum_18(int n);

std::vector<int> primes(int n);
std::vector<int> sophiegermain(int MAX_SIZE);
std::vector<int> triangle(int MAX_SIZE);
std::vector<int> square(int MAX_SIZE);
std::vector<int> cube(int MAX_SIZE);
std::vector<int> fourth_power(int MAX_SIZE);
std::vector<int> fifth_power(int MAX_SIZE);
std::vector<int> pronic(int MAX_SIZE);
std::vector<int> pentagonal(int MAX_SIZE);
std::vector<int> hexagonal(int MAX_SIZE);
std::vector<int> heptagonal(int MAX_SIZE);
std::vector<int> octagonal(int MAX_SIZE);
std::vector<int> nonagonal(int MAX_SIZE);
std::vector<int> decagonal(int MAX_SIZE);
std::vector<int> hendecagonal(int MAX_SIZE);
std::vector<int> dodecagonal(int MAX_SIZE);
std::vector<int> icosahedral(int MAX_SIZE);

std::vector<int> palindrome(int MAX_SIZE);
std::vector<int> palindrome_dsum_7(int MAX_SIZE);
std::vector<int> palindrome_dsum_18(int MAX_SIZE);
std::vector<int> palindrome_dsum_gt_10(int MAX_SIZE);

std::vector<int> product_distinct_prime(int MAX_SIZE);
std::vector<int> power_2_backwards(int MAX_SIZE);
std::vector<int> power_2(int MAX_SIZE);
std::vector<int> factorial_diff(int MAX_SIZE);

std::vector<int> permutation_12345_monotonic_seq_four(int max_size);
#endif