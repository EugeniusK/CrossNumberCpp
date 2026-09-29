#include "solver_one.h"

#include <algorithm>

#include "utils.h"
void SolverOne::solve(ArrayCrossNumber crossnumber) {
  int guesses[MAX_HINT_COUNT] = {0};
  int number_guesses = 0;
  bool forward = true;
  bool increment = false;
  bool backtrack = false;

  bool stack_full;
  bool stack_can_increment_top;

  std::vector<std::array<int, MAX_HINT_COUNT>> solutions;

  BacktrackStack stack = BacktrackStack();

  std::sort(crossnumber.hints.begin(), crossnumber.hints.end(),
            [](const Hint& a, const Hint& b) {
              return a.number_possible_values < b.number_possible_values;
            });

  while (true) {
    if (forward) {
      // expand, push 0 to current guess
      stack.push(0);
      guesses[number_guesses] = 0;
      number_guesses++;
    } else if (increment) {
      // add 1 to most recent guess
      stack.push(stack.pop() + 1);
      guesses[number_guesses - 1] += 1;
    } else if (backtrack) {
      // remove most recent guess
      stack.pop();
      number_guesses--;
    } else {
      throw std::logic_error("Not forward, increment or backtrack");
    }

    if (stack.get_size() == 0) {
      break;
    }

    stack_full = stack.get_size() == crossnumber.hints.size();
    stack_can_increment_top =
        stack.get_top() + 1 <
        crossnumber.hints[stack.get_size() - 1].get().number_possible_values;

    if (backtrack) {
      // if just backtracked, keep backtracking if top can't be incremented
      forward = false;
      increment = stack_can_increment_top;
      backtrack = !stack_can_increment_top;
    } else {
      // has just forward or increment
      if (crossnumber.try_values(guesses, number_guesses)) {
        if (number_guesses == crossnumber.hints.size()) {
          crossnumber.apply_values(guesses, number_guesses);
          if (get_nth_digit(crossnumber.get_value(1, 5, 3, true), 1, 3) ==
              19 - crossnumber.count_digits(9)) {
            if (crossnumber.get_value(8, 6, 2, true) ==
                dsum(crossnumber.get_value(0, 4, 2, true))) {
              std::cout << crossnumber.display_value() << std::endl;
              std::cout << crossnumber.display_digit_count() << std::endl;

            }
          };
          crossnumber.clear_values(guesses, number_guesses);
        }
        // guess suceeded, go forward if possible
        forward = !stack_full;
        // if going forward not possible, either increment or backtrack
        increment = stack_full && stack_can_increment_top;
        backtrack = stack_full && !stack_can_increment_top;
      } else {
        // guess failed, then increment or backtrack
        forward = false;
        increment = stack_can_increment_top;
        backtrack = !stack_can_increment_top;
      }
    }
  }

  crossnumber.apply_values(guesses, number_guesses);
  // std::cout << crossnumber.display_value() << std::endl;
  // std::cout << crossnumber.display_digit_count() << std::endl;
  crossnumber.clear_values(guesses, number_guesses);

}