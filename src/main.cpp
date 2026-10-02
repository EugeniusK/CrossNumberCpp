#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "crossnumber.h"
#include "hint.h"
#include "puzzle_loader.h"
#include "solver_one.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <puzzle_file>\n";
    return 1;
  }

  try {
    PuzzleData puzzle = load_puzzle(argv[1]);
    const std::vector<int>& ryder = puzzle.grid;
    int width = puzzle.width;
    int height = puzzle.height;

    ArrayCrossNumber b = ArrayCrossNumber(width, height);
    SolverOne solver_one;

    b.define_squares(ryder);

    // Collect and sort clue IDs (across first, then down; ascending numerical
    // order)
    std::vector<std::string> clue_ids;
    clue_ids.reserve(puzzle.clue_scripts.size());
    for (const auto& [id, _] : puzzle.clue_scripts) {
      clue_ids.push_back(id);
    }
    std::sort(clue_ids.begin(), clue_ids.end(),
              [](const std::string& a, const std::string& b) {
                char ta = std::tolower(static_cast<unsigned char>(a[0]));
                char tb = std::tolower(static_cast<unsigned char>(b[0]));
                if (ta != tb) {
                  return ta < tb;  // 'a' before 'd'
                }
                return std::stoi(a.substr(1)) < std::stoi(b.substr(1));
              });

    std::vector<std::unique_ptr<Hint>> hints;
    hints.reserve(clue_ids.size());

    for (const auto& id : clue_ids) {
      bool is_horizontal =
          (std::tolower(static_cast<unsigned char>(id[0])) == 'a');
      int identifier = std::stoi(id.substr(1));
      const std::string& script = puzzle.clue_scripts.at(id);

      auto hint = std::make_unique<Hint>(identifier, is_horizontal, script);
      b.load_hint(*hint);
      hints.push_back(std::move(hint));
    }

    std::cout << "start" << std::endl;

    while (true) {
      if (b.digit_shake()) {
        break;
      }
    }
    b.digit_shake_with_dependencies();
    while (true) {
      if (b.digit_shake()) {
        break;
      }
    }
    solver_one.solve(b);
    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
