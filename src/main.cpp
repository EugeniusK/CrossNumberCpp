
#include <chrono>

#include "crossnumber.h"
#include "hints.h"
#include "lexer_new.h"
#include "numbers.h"
#include "parser_new.h"
#include "solver_one.h"

// std::vector<int> filter_candidates(const std::vector<int>& input_candidates,
//                                    ExprNode& condition, Context ctx) {
//   std::vector<int> passed;
//   for (int candidate : input_candidates) {
//     ctx["x"] = candidate;  // bind current candidate to variable 'x'
//     if (condition.evaluate(ctx) != 0) {
//       passed.push_back(candidate);
//     }
//   }
//   return passed;
// }

int main() {
  // Context c;
  // Workspace w = Workspace(100);
  // NumberNode n = NumberNode(5);
  // // std::cout << n.evaluate(c) << std::endl;

  // // w.set(5, true);
  // // w.set(6, false);

  // // std::cout << w.get(5) << std::endl;
  // // std::cout << w.get(6) << std::endl;

  // Lexer l = Lexer("a = 5; for (i = 0; i < 5; i=i+1) a = i");
  // std::string output;
  // do {
  //   std::cout << output << std::endl;
  // } while ((output = print_token(l.next_token())) != "End");

  // std::string script = R"(
  //       let total = 0;
  //       let is_active = true;

  //       for (let i = 1; i <= 6; i = i + 1) {
  //           output[i] = 5;
  //           tmp[i] = 6;
  //           let is_even = (i % 2 == 0);

  //           if (is_even && is_active) {
  //               total = total + (i * 10);
  //           } else {
  //               total = total - 1;
  //           }
  //       }

  //       print(total);
  //       let tmp = 5;
  //       tmp = +7;
  //       let a = 1;
  //       print(!a);
  //       print(tmp);
  //       print(is_prime(5));
  //       print(is_prime(4));

  //       print(isqrt(9));
  //       print(isqrt(10));
  //       while (tmp < 10) {
  //         print(tmp);
  //         tmp = tmp + 1;
  //       }

  //   )";

  // std::cout << "Executing DSL script...\n";
  // Parser parser = Parser(Lexer(script));
  // auto program = parser.parse_program();
  // std::cout << "variables used: " << parser.print_list_variables() <<
  // std::endl; std::cout << "parsed" << std::endl; Environment env =
  // Environment();

  // env.initialise_tmp_array(100, 0);
  // env.initialise_output_array(100, 0);

  // program->execute(env);

  // std::cout << "Script completed.\n";
  // std::cout << env.get_var("total") << std::endl;
  // std::cout << env.get_var("TMP_ARRAY_LENGTH") << std::endl;
  // for (int i = 0; i < 10; i++) {
  //   std::cout << env.get_output_array(i);
  // };
  // std::cout << std::endl;

  // return 0;
  // Parser p = Parser(l);
  // std::unique_ptr<ExprNode> ast = p.parse();

  // std::vector<int> candidates;
  // for (int i = 100; i <= 999; ++i) candidates.push_back(i);
  // std::string formula = "(x % 7 == 3)";

  // Parser parser = Parser(Lexer(formula));
  // std::unique_ptr<ExprNode> ast = parser.parse();

  // // 3. Suppose clue A was previously solved as 200
  // Context context;
  // context["A"] = 200;

  // std::vector<int> filtered = filter_candidates(candidates, *ast, context);

  // std::cout << "Candidates matching \"" << formula
  //           << "\" with A=" << context["A"] << ":\n";
  // for (int val : filtered) {
  //   std::cout << val << " ";
  // }
  // std::cout << "\n";

  ArrayCrossNumber b = ArrayCrossNumber(10, 10);
  SolverOne solver_one;

  // square
  Hint a1 = Hint(1, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  // multiple 1102
  Hint a3 = Hint(3, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 1102 == 0) {
        output[i] = 1;
      }
    }
  )"));
  // digit sum 9
  Hint a7 = Hint(7, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (dsum(i) == 9) {
        output[i] = 1;
      }
    }
  )"));
  // square
  Hint a8 = Hint(8, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  // square
  Hint a9 = Hint(9, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  // square
  Hint a10 = Hint(10, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  // digit sum 18
  Hint a11 = Hint(11, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (dsum(i) == 18) {
        output[i] = 1;
      }
    }
  )"));
  // cube
  Hint a13 = Hint(13, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
        output[pow(i, 3)] = 1;
      }
    }
  )"));
  // prime
  Hint a15 = Hint(15, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_prime(i)) {
        output[i] = 1;
      }
    }
  )"));
  // prime
  Hint a18 = Hint(18, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_prime(i)) {
        output[i] = 1;
      }
    }
  )"));
  // middle digit equal to 19 - num(9)
  Hint a19 = Hint(19, true, std::string(R"(
    for (let i = OUTPUT_ARRAY_LENGTH / 10; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (get_nth_digit(i, 2) == 19 - c9) {
        output[i] = true;
      }
    }
  )"));
  // difference factorials
  Hint a20 = Hint(20, true, std::string(R"(
    let a = 1;
    let b = 1;
    let a_n = 1;
    let b_n = 1;
    while (a < OUTPUT_ARRAY_LENGTH * 10) {
      b = 1;
      b_n = 1;
      while (b < OUTPUT_ARRAY_LENGTH * 10) {
        if ((a-b > 0) && (a-b < OUTPUT_ARRAY_LENGTH)) {
          output[a - b] = 1;
        }
        b = b * b_n;
        b_n = b_n + 1;
      }
      a = a * a_n;
      a_n = a_n + 1;
    }
  )"));
  // digit sum 13
  Hint a21 = Hint(21, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (dsum(i) == 13) {
        output[i] = 1;
      }
    }
  )"));
  // cube
  Hint a22 = Hint(22, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
        output[pow(i, 3)] = 1;
      }
    }
  )"));
  Hint a23 = Hint(23, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i == dsum(a15)) {
        output[i] = 1;
      }
    }
  )"));
  // product distinct primes
  Hint a25 = Hint(25, true, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_prime(i)) {
        for (let j = 0; i * j < OUTPUT_ARRAY_LENGTH; j = j + 1) {
          if (is_prime(j) && i != j) {
            output[i * j] = 1;
          }
        }
      }
    }
  )"));
  // square
  Hint a26 = Hint(26, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  // square
  Hint a27 = Hint(27, true, std::string(R"(
    let i = 0;
    while (i * i < OUTPUT_ARRAY_LENGTH) {
      output[i * i] = 1;
      i = i + 1;
    }
  )"));
  Hint a28 = Hint(28, true, std::string(R"(
    let i = 0;
    while (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 3)] = 1;
      i = i + 1;
    }
  )"));
  Hint a29 = Hint(29, true, std::string(R"(
    let i = 0;
    while (pow(i, 4) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 4)] = 1;
      i = i + 1;
    }
  )"));

  // palindrome and digit sum 18
  Hint d1 = Hint(1, false, R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_palindrome(i) && dsum(i) == 18) {
        output[i] = 1;
      }
    }
  )");
  // power of 2 reversed
  Hint d2 = Hint(2, false, R"(
    let i = 0;
    while (pow(2, i) < OUTPUT_ARRAY_LENGTH) {
      output[reverse(pow(2, i))] = 1;
      i = i + 1;
    }
  )");
  // product distinct primes
  Hint d3 = Hint(3, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_prime(i)) {
        for (let j = 0; i * j < OUTPUT_ARRAY_LENGTH; j = j + 1) {
          if (is_prime(j) && i != j) {
            output[i * j] = 1;
          }
        }
      }
    }
  )"));
  // power of 2
  Hint d4 = Hint(4, false, R"(
    let i = 0;
    while (pow(2, i) < OUTPUT_ARRAY_LENGTH) {
      output[pow(2, i)] = 1;
      i = i + 1;
    }
  )");
  // cube
  Hint d5 = Hint(5, false, std::string(R"(
    let i = 0;
    while (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 3)] = 1;
      i = i + 1;
    }
  )"));
  // multiple 11111
  Hint d6 = Hint(6, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 11111 == 0) {
        output[i] = 1;
      }
    }
  )"));
  // multiple 11111
  Hint d12 = Hint(12, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 11111 == 0) {
        output[i] = 1;
      }
    }
  )"));
  // palindrome and digit sum greater than 10
  Hint d14 = Hint(14, false, R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_palindrome(i) && dsum(i) > 10) {
        output[i] = 1;
      }
    }
  )");
  // cube
  Hint d16 = Hint(16, false, std::string(R"(
    let i = 0;
    while (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 3)] = 1;
      i = i + 1;
    }
  )"));
  // multiple 7
  Hint d17 = Hint(17, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 7 == 0) {
        output[i] = 1;
      }
    }
  )"));
  Hint d20 = Hint(20, false);
  // multiple 2020
  Hint d21 = Hint(21, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 2020 == 0) {
        output[i] = 1;
      }
    }
  )"));
  // power five
  Hint d24 = Hint(24, false, std::string(R"(
    let i = 0;
    while (pow(i, 5) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 5)] = 1;
      i = i + 1;
    }
  )"));
  // cube
  Hint d25 = Hint(25, false, std::string(R"(
    let i = 0;
    while (pow(i, 3) < OUTPUT_ARRAY_LENGTH) {
      output[pow(i, 3)] = 1;
      i = i + 1;
    }
  )"));
  // palindrome and digit sum 7
  Hint d26 = Hint(26, false, R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (is_palindrome(i) && dsum(i) == 7) {
        output[i] = 1;
      }
    }
  )");
  // multiple 3
  Hint d28 = Hint(28, false, std::string(R"(
    for (let i = 0; i < OUTPUT_ARRAY_LENGTH; i= i + 1) {
      if (i % 3 == 0) {
        output[i] = 1;
      }
    }
  )"));

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

  // 36 | 44 | 9 | 4 | 8 | 10 | 4 | 5 | 3 | 10 |
  // a1.load(square);
  // a3.load(multiple_1102);
  // a7.load(dsum_9);
  // a8.load(square);
  // a9.load(square);
  // a10.load(square);
  // a11.load(dsum_18);
  // a13.load(cube);
  // a15.load(primes);
  // a18.load(primes);
  // a19 - middle digit equal to 19 - num(9)
  // a20.load(factorial_diff);
  // a21.load(dsum_13);
  // a22.load(cube);
  // a23 - dsum(a15)
  // a25.load(product_distinct_prime);
  // a26.load(square);
  // a27.load(square);
  // a28.load(cube);
  // a29.load(fourth_power);

  // d1.load(palindrome_dsum_18);
  // d2.load(power_2_backwards);
  // d3.load(product_distinct_prime);
  // d4.load(power_2);
  // d5.load(cube);
  // d6.load(multiple_11111);
  // d12.load(multiple_11111);
  // d14.load(palindrome_dsum_gt_10);
  // d16.load(cube);
  // d17.load(multiple_7);
  d20.load(permutation_12345_monotonic_seq_four);
  // d21.load(multiple_2020);
  // d24.load(fifth_power);
  // d25.load(cube);
  // d26.load(palindrome_dsum_7);
  // d28.load(multiple_3);
  std::cout << "start" << std::endl;

  while (true) {
    if (b.digit_shake()) {
      break;
    }
  }

  solver_one.solve(b);
  return 0;
}
