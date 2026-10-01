#include "numbers.h"

#include <iostream>

#include "utils.h"
// import math, json
// import itertools as iter

std::vector<int> multiple_3(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * 3 < MAX_SIZE) {
    arr.push_back(n * 3);
    n++;
  }
  return arr;
}

std::vector<int> multiple_7(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * 7 < MAX_SIZE) {
    arr.push_back(n * 7);
    n++;
  }
  return arr;
}

std::vector<int> multiple_1102(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * 1102 < MAX_SIZE) {
    arr.push_back(n * 1102);
    n++;
  }
  return arr;
}

std::vector<int> multiple_2020(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * 2020 < MAX_SIZE) {
    arr.push_back(n * 2020);
    n++;
  }
  return arr;
}

std::vector<int> multiple_11111(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * 11111 < MAX_SIZE) {
    arr.push_back(n * 11111);
    n++;
  }
  return arr;
}

std::vector<int> dsum_9(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n < MAX_SIZE) {
    if (dsum(n) == 9) {
      arr.push_back(n);
    }
    n++;
  }
  return arr;
}

std::vector<int> dsum_13(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n < MAX_SIZE) {
    if (dsum(n) == 13) {
      arr.push_back(n);
    }
    n++;
  }
  return arr;
}

std::vector<int> dsum_18(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n < MAX_SIZE) {
    if (dsum(n) == 18) {
      arr.push_back(n);
    }
    n++;
  }
  return arr;
}

std::vector<int> primes(int n) {
  std::vector<bool> isPrime(n, true);
  int p = 2;
  while (p * p <= n) {
    if (isPrime[p]) {
      for (int i = p * p; i < n + 1; i += p) {
        isPrime[i] = false;
      }
    }
    p++;
  }
  std::vector<int> result;
  for (int i = 2; i < n; i++) {
    if (isPrime[i]) {
      result.push_back(i);
    }
  }
  return result;
}

std::vector<int> prime_arr = primes(1000000);

std::vector<int> sophiegermain(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n < MAX_SIZE) {
    if (std::find(prime_arr.begin(), prime_arr.end(), n) != prime_arr.end() &&
        std::find(prime_arr.begin(), prime_arr.end(), 2 * n + 1) !=
            prime_arr.end()) {
      arr.push_back(n);
    }
    n++;
  }
  return arr;
}
// def sophiegermain(MAX_SIZE):
//     # sophiegermain
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         if n in prime_arr and 2 * n + 1 in prime_arr:
//             arr.append(n)
//         n += 1

//     return sorted(arr)

// def tetradic(MAX_SIZE):
//     # tetradic
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         if (
//             set(list(str(n))).intersection({"2", "3", "4", "5", "6", "7",
//             "9"}) == set() and str(n) == str(n)[::-1]
//         ):
//             arr.append(n)
//         n += 1

//     return sorted(arr)

// def quartan(MAX_SIZE):
//     # quartan
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         tmp = 1
//         while True:
//             if tmp**4 > n:
//                 break
//             if (
//                 n in prime_arr
//                 and math.isqrt(math.isqrt(n - tmp**4)) ** 4 + tmp**4 == n
//             ):
//                 arr.append(n)
//                 break
//             tmp += 1
//         n += 1

//     return sorted(arr)

// def emirp(MAX_SIZE):
//     # emirp
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         if n in prime_arr and int(str(n)[::-1]) in prime_arr and
//         int(str(n)[::-1]) != n:
//             arr.append(n)
//         n += 1

//     return sorted(arr)

std::vector<int> triangle(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (n + 1) / 2 < MAX_SIZE) {
    arr.push_back(n * (n + 1) / 2);
    n++;
  }
  return arr;
}

std::vector<int> square(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * n < MAX_SIZE) {
    arr.push_back(n * n);
    n++;
  }
  return arr;
}

std::vector<int> cube(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * n * n < MAX_SIZE) {
    arr.push_back(n * n * n);
    n++;
  }
  return arr;
}

std::vector<int> fourth_power(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * n * n * n < MAX_SIZE) {
    arr.push_back(n * n * n * n);
    n++;
  }
  return arr;
}

std::vector<int> fifth_power(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * n * n * n * n < MAX_SIZE) {
    arr.push_back(n * n * n * n * n);
    n++;
  }
  return arr;
}

std::vector<int> pronic(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (n + 1) < MAX_SIZE) {
    arr.push_back(n * (n + 1));
    n++;
  }
  return arr;
}

std::vector<int> pentagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (3 * n - 1) / 2 < MAX_SIZE) {
    arr.push_back(n * (3 * n - 1) / 2);
    n++;
  }
  return arr;
}

std::vector<int> hexagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (2 * n - 1) < MAX_SIZE) {
    arr.push_back(n * (2 * n - 1));
    n++;
  }
  return arr;
}

std::vector<int> heptagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (5 * n - 3) / 2 < MAX_SIZE) {
    arr.push_back(n * (5 * n - 3) / 2);
    n++;
  }
  return arr;
}

std::vector<int> octagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (ipow(2 * n + 1, 2) < MAX_SIZE) {
    arr.push_back(ipow(2 * n + 1, 2));
    n++;
  }
  return arr;
}

std::vector<int> nonagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (7 * n - 5) / 2 < MAX_SIZE) {
    arr.push_back(n * (7 * n - 5) / 2);
    n++;
  }
  return arr;
}

std::vector<int> decagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (4 * n - 3) < MAX_SIZE) {
    arr.push_back(n * (4 * n - 3));
    n++;
  }
  return arr;
}

std::vector<int> hendecagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (9 * n - 7) / 2 < MAX_SIZE) {
    arr.push_back(n * (9 * n - 7) / 2);
    n++;
  }
  return arr;
}

std::vector<int> dodecagonal(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (6 * n * (n - 1) + 1 < MAX_SIZE) {
    arr.push_back(6 * n * (n - 1) + 1);
    n++;
  }
  return arr;
}

std::vector<int> icosahedral(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (5 * n * n - 5 * n + 2) / 2 < MAX_SIZE) {
    arr.push_back(n * (5 * n * n - 5 * n + 2) / 2);
    n++;
  }
  return arr;
}

// # def all_polygonal(MAX_SIZE):
// #     arr = dict()
// #     tmp = 0
// #     s = 2
// #     while s <= MAX_SIZE:
// #         n = 0
// #         tmp = 0
// #         tmp_arr = []
// #         while tmp < MAX_SIZE:
// #             tmp = ((s - 2) * n**2 - (s - 4) * n) // 2
// #             if tmp < MAX_SIZE:
// #                 tmp_arr.append(tmp)
// #             n += 1
// #         arr[s] = tmp_arr
// #         s += 1
// #     return arr

std::vector<int> tetrahedral(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (n + 1) * (n + 2) / 6 < MAX_SIZE) {
    arr.push_back(n * (n + 1) * (n + 2) / 6);
    n++;
  }
  return arr;
}

std::vector<int> octahedral(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (n * (2 * n * n + 1) / 3 < MAX_SIZE) {
    arr.push_back(n * (2 * n * n + 1) / 3);
    n++;
  }
  return arr;
}

// def padovan(MAX_SIZE):
//     # padovan
//     arr = [1, 0, 0]
//     n = sum(arr[-3:-1])
//     while n < MAX_SIZE:
//         arr.append(n)
//         n = sum(arr[-3:-1])

//     return sorted(list(set(arr)))

// def lucas(MAX_SIZE):
//     arr = [2, 1]
//     n = sum(arr[-2:])
//     while n < MAX_SIZE:
//         arr.append(n)
//         n = sum(arr[-2:])

//     return sorted(list(set(arr)))

// def perrin(MAX_SIZE):
//     arr = [3, 0, 2]
//     n = sum(arr[-3:-1])
//     while n < MAX_SIZE:
//         arr.append(n)
//         n = sum(arr[-3:-1])

//     return sorted(list(set(arr)))

// def tribonacci(MAX_SIZE):
//     arr = [9, 12, -10]
//     n = sum(arr[-3:])
//     while n < MAX_SIZE:
//         arr.append(n)
//         n = sum(arr[-3:])

//     return sorted(list(set(arr)))

// def keith(MAX_SIZE):
//     arr = []
//     n = 10
//     while n < MAX_SIZE:
//         tmp_arr = [int(x) for x in str(n)]
//         len_n = len(str(n))
//         while tmp_arr[-1] < n:
//             tmp_arr.append(sum(tmp_arr[-len_n:]))
//         if n in tmp_arr:
//             arr.append(n)
//         n += 1

//     return sorted(arr)

// def super3(MAX_SIZE):
//     # super3
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         if "333" in str(3 * n**3):
//             arr.append(n)
//         n += 1

//     return sorted(list(set(arr)))

// def super4(MAX_SIZE):
//     arr = []
//     n = 0
//     while n < MAX_SIZE:
//         if "4444" in str(4 * n**4):
//             arr.append(n)
//         n += 1

//     return sorted(list(set(arr)))

// def kaprekar(MAX_SIZE):
//     # kaprekar
//     arr = []
//     n = 0

//     def iskaprekar(n):
//         # taken from geeksforgeeks
//         if n == 1:
//             return True

//         # Count number of digits in square
//         sq_n = n * n
//         count_digits = 1
//         while not sq_n == 0:
//             count_digits = count_digits + 1
//             sq_n = sq_n // 10

//         sq_n = n * n  # Recompute square as it was changed

//         # Split the square at different points and see if sum
//         # of any pair of splitted numbers is equal to n.
//         r_digits = 0
//         while r_digits < count_digits:
//             r_digits = r_digits + 1
//             eq_parts = (int)(math.pow(10, r_digits))

//             # To avoid numbers like 10, 100, 1000 (These are not
//             # Kaprekar numbers
//             if eq_parts == n:
//                 continue

//             # Find sum of current parts and compare with n

//             sum = sq_n // eq_parts + sq_n % eq_parts
//             if sum == n:
//                 return True

//         # compare with original number
//         return False

//     while n < MAX_SIZE:
//         if iskaprekar(n):
//             arr.append(n)
//         n += 1

//     return sorted(list(set(arr)))

// def vampire(MAX_SIZE):
//     # vampire
//     arr = []
//     n = 1000
//     while n < MAX_SIZE:
//         list_n = sorted(list(str(n)))
//         for x in range(1, math.isqrt(n) + 1):
//             if n % x == 0 and sorted(list(str(x)) + list(str(n // x))) ==
//             list_n:
//                 arr.append(n)
//                 break
//         n += 1

//     return sorted(list(set(arr)))

// def untouchable(MAX_SIZE):
//     # untouchable
//     arr = list(range(1, MAX_SIZE + 1))

//     def sigma(n):
//         """
//         Sum of divisors
//         """
//         divisors = []
//         for x in range(1, math.isqrt(n) + 1):
//             if n % x == 0:
//                 divisors.append(x)
//                 divisors.append(n // x)
//         return sum(list(set(divisors)))

//     for x in range(MAX_SIZE**2):
//         result = sigma(x) - x
//         if result in arr:
//             arr.remove(result)
//     return sorted(list(set(arr)))

// def trimorphic(MAX_SIZE):
//     arr = []
//     n = 1
//     while n < MAX_SIZE:
//         if str(n**3)[-len(str(n)) :] == str(n):
//             arr.append(n)
//         n += 1

//     return sorted(list(set(arr)))

// def strobogrammatic(MAX_SIZE):
//     # strobogrammatic
//     arr = []

//     def numdef(n, length):
//         if n == 0:
//             return [""]
//         if n == 1:
//             return ["1", "0", "8"]

//         middles = numdef(n - 2, length)
//         result = []

//         for middle in middles:
//             if n != length:
//                 result.append("0" + middle + "0")

//             result.append("8" + middle + "8")
//             result.append("1" + middle + "1")
//             result.append("9" + middle + "6")
//             result.append("6" + middle + "9")
//         return result

//     arr = [
//         int(item)
//         for row in [numdef(x, x) for x in range(1, len(str(MAX_SIZE)))]
//         for item in row
//     ]

//     return sorted(arr)

// def integer(MAX_SIZE):
//     return list(range(1, MAX_SIZE))

// def A053873(MAX_SIZE):
//     return [x for x in [53873, 53169] if x < MAX_SIZE]

// def palindrome(MAX_SIZE):
//     return sorted(list(set([x for x in range(MAX_SIZE) if str(x) ==
//     str(x)[::-1]])))

std::vector<int> palindrome(int MAX_SIZE) {
  std::vector<int> arr;
  for (int i = 0; i < MAX_SIZE; i++) {
    if (reverse_num(i) == i) {
      arr.push_back(i);
    }
  }

  return arr;
}

std::vector<int> palindrome_dsum_7(int MAX_SIZE) {
  std::vector<int> arr;
  for (int i = 0; i < MAX_SIZE; i++) {
    if (reverse_num(i) == i && dsum(i) == 7) {
      arr.push_back(i);
    }
  }

  return arr;
}

std::vector<int> palindrome_dsum_18(int MAX_SIZE) {
  std::vector<int> arr;
  for (int i = 0; i < MAX_SIZE; i++) {
    if (reverse_num(i) == i && dsum(i) == 18) {
      arr.push_back(i);
    }
  }

  return arr;
}

std::vector<int> palindrome_dsum_gt_10(int MAX_SIZE) {
  std::vector<int> arr;
  for (int i = 0; i < MAX_SIZE; i++) {
    if (reverse_num(i) == i && dsum(i) > 10) {
      arr.push_back(i);
    }
  }

  return arr;
}

std::vector<int> product_distinct_prime(int MAX_SIZE) {
  std::vector<int> arr;
  for (int p : prime_arr) {
    for (int q : prime_arr) {
      if (p * q < MAX_SIZE) {
        if (p != q) {
          arr.push_back(p * q);
        }
      } else {
        break;
      }
    }
  }
  return arr;
}

// def product_distinct_prime(MAX_SIZE):
//     arr = []
//     for p in prime_arr:
//         for q in prime_arr:
//             if p * q < MAX_SIZE:
//                 if p != q:
//                     arr.append(p * q)
//             else:
//                 break
//     return sorted(list(set(arr)))

// def power_2_backwards(MAX_SIZE):
//     n = 1
//     arr = []
//     while True:
//         if int(str(2**n)[::-1]) < MAX_SIZE:
//             arr.append(int(str(2**n)[::-1]))
//         else:
//             break
//         n += 1
//     return arr

std::vector<int> power_2_backwards(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (reverse_num(ipow(2, n)) < MAX_SIZE) {
    arr.push_back(reverse_num(ipow(2, n)));
    n++;
  }
  return arr;
}

// def power_2(MAX_SIZE):
//     n = 1
//     arr = []
//     while True:
//         if 2**n < MAX_SIZE:
//             arr.append(2**n)
//         else:
//             break
//         n += 1
//     return arr

std::vector<int> power_2(int MAX_SIZE) {
  std::vector<int> arr;
  int n = 0;
  while (ipow(2, n) < MAX_SIZE) {
    arr.push_back(ipow(2, n));
    n++;
  }
  return arr;
}

// def prime(MAX_SIZE):
//     return [x for x in prime_arr if x < MAX_SIZE]

// def factorial_diff(MAX_SIZE):
//     arr = []
//     fact = [1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
//     479001600] for a in fact:
//         for b in fact:
//             if a - b < MAX_SIZE and a - b > 0:
//                 arr.append(a - b)
//     return sorted(list(set(arr)))

std::vector<int> factorial_diff(int MAX_SIZE) {
  std::vector<int> arr;
  std::vector<int> factorial = {1,       1,        2,        6,     24,
                                120,     720,      5040,     40320, 362880,
                                3628800, 39916800, 479001600};

  for (int a : factorial) {
    for (int b : factorial) {
      if (a - b < MAX_SIZE && a - b > 0) {
        arr.push_back(a - b);
      }
    }
  }
  return arr;
}
std::vector<int> permutation_12345_monotonic_seq_four(int max_size) {
  std::vector<int> result;
  std::vector<int> digits = {1, 2, 3, 4, 5};

  do {
    int n = digits.size();
    std::vector<int> dp_inc(n, 1);
    std::vector<int> dp_dec(n, 1);
    int max_lis = 1;
    int max_lds = 1;

    // Calculate LIS and LDS lengths
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < i; j++) {
        if (digits[i] > digits[j]) {
          dp_inc[i] = std::max(dp_inc[i], dp_inc[j] + 1);
        } else if (digits[i] < digits[j]) {
          dp_dec[i] = std::max(dp_dec[i], dp_dec[j] + 1);
        }
      }
      max_lis = std::max(max_lis, dp_inc[i]);
      max_lds = std::max(max_lds, dp_dec[i]);
    }

    // Filter for exactly length 4
    if (std::max(max_lis, max_lds) == 4) {
      int perm_as_number = 0;
      for (int d : digits) {
        perm_as_number = perm_as_number * 10 + d;
      }
      result.push_back(perm_as_number);
    }

  } while (std::next_permutation(digits.begin(), digits.end()));

  return result;
}

// def permutation_12345_monotonic_seq_four(MAX_SIZE):
//     arr = []
//     for n in range(1, int(math.log10(MAX_SIZE)) + 1):
//         arr.extend([x for x in iter.product("12345", repeat=n) if len(set(x))
//         != 1])

//     return [int("".join(x)) for x in arr]

// NUMBERS_DICT = {
//     "triangle": triangle,
//     "square": square,
//     "pronic": pronic,
//     "pentagonal": pentagonal,
//     "hexagonal": hexagonal,
//     "heptagonal": heptagonal,
//     "octagonal": octagonal,
//     "nonagonal": nonagonal,
//     "decagonal": decagonal,
//     "dodecagonal": dodecagonal,
//     "icosahedral": icosahedral,
//     "tetrahedral": tetrahedral,
//     "octahedral": octahedral,
//     "padovan": padovan,
//     "lucas": lucas,
//     "perrin": perrin,
//     "tribonacci": tribonacci,
//     "keith": keith,
//     "super3": super3,
//     "super4": super4,
//     "kaprekar": kaprekar,
//     "vampire": vampire,
//     "untouchable": untouchable,
//     "strobogrammatic": strobogrammatic,
//     "trimorphic": trimorphic,
//     "integer": integer,
//     "sophiegermain": sophiegermain,
//     "tetradic": tetradic,
//     "quartan": quartan,
//     "emirp": emirp,
//     "A053873": A053873,
//     "integer": integer,
//     "square": square,
//     "cube": cube,
//     "4th_power": fourth_power,
//     "5th_power": fifth_power,
//     "palindrome": palindrome,
//     "product_distinct_prime": product_distinct_prime,
//     "power_2_backwards": power_2_backwards,
//     "power_2": power_2,
//     "prime": prime,
//     "factorial_diff": factorial_diff,
//     "permutation_12345_monotonic_seq_four":
//     permutation_12345_monotonic_seq_four,
// }
// if __name__ == "__main__":
//     print("main")