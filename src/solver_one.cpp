#include "solver_one.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "backtrackstack.h"

static std::string format_number(long long n) {
  if (n < 0) return "-" + format_number(-n);
  std::string s = std::to_string(n);
  int insert_pos = static_cast<int>(s.length()) - 3;
  while (insert_pos > 0) {
    s.insert(insert_pos, ",");
    insert_pos -= 3;
  }
  return s;
}

void SolverOne::solve(ArrayCrossNumber crossnumber) {
  std::vector<int> guesses(crossnumber.hints.size(), 0);
  int number_guesses = 0;
  bool forward = true;
  bool increment = false;
  bool backtrack = false;

  bool stack_full;
  bool stack_can_increment_top;

  BacktrackStack stack;

  std::sort(crossnumber.hints.begin(), crossnumber.hints.end(),
            [](const Hint& a, const Hint& b) {
              return a.number_possible_values < b.number_possible_values;
            });

  auto start_time = std::chrono::steady_clock::now();
  auto last_update_time = start_time;
  long long combinations_explored = 0;
  double max_progress = 0.0;

  auto compute_progress = [&]() -> double {
    double prog = 0.0;
    double weight = 1.0;
    for (size_t i = 0; i < crossnumber.hints.size(); ++i) {
      int total_m = crossnumber.hints[i].get().number_possible_values;
      if (total_m <= 0) break;
      int g = (i < static_cast<size_t>(number_guesses)) ? guesses[i] : 0;
      prog += (static_cast<double>(g) / total_m) * weight;
      weight /= total_m;
      if (weight < 1e-15) break;
    }
    return std::clamp(prog, 0.0, 1.0);
  };

  auto render_progress = [&](bool done = false) {
    auto now = std::chrono::steady_clock::now();
    double elapsed_sec =
        std::chrono::duration<double>(now - start_time).count();
    double rate = elapsed_sec > 0.05
                      ? static_cast<double>(combinations_explored) / elapsed_sec
                      : 0.0;

    double prog = done ? 1.0 : max_progress;
    double pct = prog * 100.0;

    const int bar_width = 25;
    int filled = std::clamp(static_cast<int>(prog * bar_width), 0, bar_width);
    std::string bar = "";
    for (int i = 0; i < filled; ++i) bar += "█";
    for (int i = filled; i < bar_width; ++i) bar += "░";

    std::cout << "\r[Solver] [" << bar << "] " << std::fixed
              << std::setprecision(1) << pct << "% | "
              << "Explored: " << format_number(combinations_explored) << " | "
              << "Depth: " << number_guesses << "/" << crossnumber.hints.size();

    if (rate >= 1e6) {
      std::cout << " (" << std::fixed << std::setprecision(2) << (rate / 1e6)
                << "M/s)";
    } else if (rate >= 1e3) {
      std::cout << " (" << std::fixed << std::setprecision(1) << (rate / 1e3)
                << "k/s)";
    } else {
      std::cout << " (" << static_cast<int>(rate) << "/s)";
    }
    std::cout << "    " << std::flush;
  };

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
      combinations_explored++;
      if ((combinations_explored & 1023) == 0) {
        auto now = std::chrono::steady_clock::now();
        if (now - last_update_time >= std::chrono::milliseconds(100)) {
          double cur_p = compute_progress();
          if (cur_p > max_progress) {
            max_progress = cur_p;
          }
          render_progress(false);
          last_update_time = now;
        }
      }

      if (crossnumber.try_values(guesses, number_guesses)) {
        if (number_guesses == crossnumber.hints.size()) {
          crossnumber.apply_values(guesses, number_guesses);

          bool with_valid_dependencies = true;
          for (Hint& h : crossnumber.hints) {
            if (h.get_if_has_dependencies()) {
              for (std::string s : h.get_dependencies()) {
                if (s[0] == 'a') {
                  for (Hint& dependent_hint : crossnumber.hints) {
                    if (dependent_hint.get_identifier() ==
                            std::stoi(s.substr(1, s.size() - 1)) &&
                        dependent_hint.get_is_horizontal()) {
                      h.set_env(s, crossnumber.get_value(dependent_hint));
                    }
                  }
                } else if (s[0] == 'd') {
                  for (Hint& dependent_hint : crossnumber.hints) {
                    if (dependent_hint.get_identifier() ==
                            std::stoi(s.substr(1, s.size() - 1)) &&
                        !dependent_hint.get_is_horizontal()) {
                      h.set_env(s, crossnumber.get_value(dependent_hint));
                    }
                  }
                } else if (s[0] == 'c') {
                  h.set_env(
                      s, crossnumber.count_digits(std::stoi(s.substr(1, 1))));
                }
              }
              h.run_program_on_dependency();
              if (h.get_output_array(crossnumber.get_value(h)) == 0) {
                with_valid_dependencies = false;
              }
            }
          }

          if (with_valid_dependencies) {
            std::cout << "\r" << std::string(100, ' ') << "\r";
            std::cout << "solved" << std::endl;
            // std::cout << crossnumber.display_value() << std::endl;
            // std::cout << crossnumber.display_digit_count() << std::endl;
            last_update_time = std::chrono::steady_clock::now();
          }

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

  std::cout << "\r" << std::string(100, ' ') << "\r";
  render_progress(true);
  std::cout << std::endl;

  crossnumber.apply_values(guesses, number_guesses);
  crossnumber.clear_values(guesses, number_guesses);
}