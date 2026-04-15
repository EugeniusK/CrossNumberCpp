
#include <chrono>

#include "crossnumber.h"
#include "hints.h"
#include "numbers.h"
#include "solver_one.h"
int main() {
  ArrayCrossNumber b = ArrayCrossNumber(10, 10);
  SolverOne solver_one;

  Hint a1 = Hint(1, true);
  Hint a3 = Hint(3, true);
  Hint a7 = Hint(7, true);
  Hint a8 = Hint(8, true);
  Hint a9 = Hint(9, true);
  Hint a10 = Hint(10, true);
  Hint a11 = Hint(11, true);
  Hint a13 = Hint(13, true);
  Hint a15 = Hint(15, true);
  Hint a18 = Hint(18, true);
  Hint a19 = Hint(19, true);
  Hint a20 = Hint(20, true);
  Hint a21 = Hint(21, true);
  Hint a22 = Hint(22, true);
  Hint a23 = Hint(23, true);
  Hint a25 = Hint(25, true);
  Hint a26 = Hint(26, true);
  Hint a27 = Hint(27, true);
  Hint a28 = Hint(28, true);
  Hint a29 = Hint(29, true);

  Hint d1 = Hint(1, false);
  Hint d2 = Hint(2, false);
  Hint d3 = Hint(3, false);
  Hint d4 = Hint(4, false);
  Hint d5 = Hint(5, false);
  Hint d6 = Hint(6, false);
  Hint d12 = Hint(12, false);
  Hint d14 = Hint(14, false);
  Hint d16 = Hint(16, false);
  Hint d17 = Hint(17, false);
  Hint d20 = Hint(20, false);
  Hint d21 = Hint(21, false);
  Hint d24 = Hint(24, false);
  Hint d25 = Hint(25, false);
  Hint d26 = Hint(26, false);
  Hint d28 = Hint(28, false);

  std::vector<int> ryder = {
      1,  0,  2,  -1, 3,  4, 0,  5,  -1, 6,  0,  -1, 7,  0,  0,  0,  -1,
      8,  0,  0,  9,  0,  0, -1, -1, 10, 0,  0,  -1, 0,  0,  -1, 11, 12,
      0,  0,  -1, 13, 14, 0, 15, 16, -1, 0,  -1, -1, 17, -1, 18, 0,  -1,
      19, 0,  0,  -1, 20, 0, 0,  0,  -1, 21, 0,  -1, 22, 0,  0,  -1, -1,
      23, 24, 0,  -1, 25, 0, -1, 0,  -1, 26, 0,  0,  27, 0,  0,  -1, 28,
      0,  0,  0,  -1, 0,  0, -1, 29, 0,  0,  0,  -1, 0,  -1, 0};

  b.define_squares(ryder);

  b.load_hint(a1);
  b.load_hint(a3);
  b.load_hint(a7);
  b.load_hint(a8);
  b.load_hint(a9);
  b.load_hint(a10);
  b.load_hint(a11);
  b.load_hint(a13);
  b.load_hint(a15);
  b.load_hint(a18);
  b.load_hint(a19);
  b.load_hint(a20);
  b.load_hint(a21);
  b.load_hint(a22);
  b.load_hint(a23);
  b.load_hint(a25);
  b.load_hint(a26);
  b.load_hint(a27);
  b.load_hint(a28);
  b.load_hint(a29);
  b.load_hint(d1);
  b.load_hint(d2);
  b.load_hint(d3);
  b.load_hint(d4);
  b.load_hint(d5);
  b.load_hint(d6);
  b.load_hint(d12);
  b.load_hint(d14);
  b.load_hint(d16);
  b.load_hint(d17);
  b.load_hint(d20);
  b.load_hint(d21);
  b.load_hint(d24);
  b.load_hint(d25);
  b.load_hint(d26);
  b.load_hint(d28);

  a1.load(square);
  a3.load(multiple_1102);
  a7.load(dsum_9);
  a8.load(square);
  a9.load(square);
  a10.load(square);
  a11.load(dsum_18);
  a13.load(cube);
  a15.load(primes);
  a18.load(primes);
  // a19 - middle digit equal to 19 - num(9)
  a20.load(factorial_diff);
  a21.load(dsum_13);
  a22.load(cube);
  // a23 - dsum(a15)
  a25.load(product_distinct_prime);
  a26.load(square);
  a27.load(square);
  a28.load(cube);
  a29.load(fourth_power);
  d1.load(palindrome_dsum_18);
  d2.load(power_2_backwards);
  d3.load(product_distinct_prime);
  d4.load(power_2);
  d5.load(cube);
  d6.load(multiple_11111);
  d12.load(multiple_11111);
  d14.load(palindrome_dsum_gt_10);
  d16.load(cube);
  d17.load(multiple_7);
  d20.load(permutation_12345_monotonic_seq_four);
  d21.load(multiple_2020);
  d24.load(fifth_power);
  d25.load(cube);
  d26.load(palindrome_dsum_7);
  d28.load(multiple_3);
  std::cout << "start" << std::endl;

  while (true) {
    if (b.digit_shake()) {break;}
  }

  
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
// b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();
  // b.digit_shake();

  solver_one.solve(b);

  return 0;
}