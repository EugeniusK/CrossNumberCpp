#ifndef PUZZLE_LOADER_H
#define PUZZLE_LOADER_H

#include <string>
#include <unordered_map>
#include <vector>

struct PuzzleData {
  int width = 0;
  int height = 0;
  std::vector<int> grid;  // Flat array matching the existing ryder layout
  std::unordered_map<std::string, std::string> clue_scripts;  // Keyed by "a1", "d1", etc.
};

PuzzleData load_puzzle(const std::string& filepath);

#endif
