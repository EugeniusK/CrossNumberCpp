#include "puzzle_loader.h"

#include <cctype>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace {
std::string trim(const std::string& s) {
  size_t start = 0;
  while (start < s.size() &&
         std::isspace(static_cast<unsigned char>(s[start]))) {
    start++;
  }
  if (start == s.size()) return "";
  size_t end = s.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(s[end - 1]))) {
    end--;
  }
  return s.substr(start, end - start);
}
}  // namespace

PuzzleData load_puzzle(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open puzzle file: " + filepath);
  }

  PuzzleData puzzle;
  bool reading_layout = false;
  std::string current_clue_id;
  std::string line;

  while (std::getline(file, line)) {
    std::string trimmed = trim(line);

    // Check for [CLUE <id>] header
    if (trimmed.size() >= 6 && trimmed.front() == '[' &&
        trimmed.back() == ']') {
      std::string inner = trim(trimmed.substr(1, trimmed.size() - 2));
      if (inner.size() >= 5 &&
          (inner.rfind("CLUE ", 0) == 0 || inner.rfind("clue ", 0) == 0)) {
        std::string clue_id = trim(inner.substr(5));
        for (char& ch : clue_id) {
          ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }
        if (puzzle.clue_scripts.find(clue_id) != puzzle.clue_scripts.end()) {
          throw std::runtime_error("Duplicate clue definition for '" +
                                   clue_id + "' in " + filepath);
        }
        reading_layout = false;
        current_clue_id = clue_id;
        puzzle.clue_scripts[current_clue_id] = "";
        continue;
      }
    }

    // Inside a clue block: preserve verbatim
    if (!current_clue_id.empty()) {
      puzzle.clue_scripts[current_clue_id] += line + "\n";
      continue;
    }

    // Outside clue blocks: skip empty lines and comments
    if (trimmed.empty() || trimmed[0] == '#') {
      continue;
    }

    // Parse GRID <width> <height>
    if (trimmed.rfind("GRID", 0) == 0) {
      std::istringstream iss(trimmed.substr(4));
      if (!(iss >> puzzle.width >> puzzle.height)) {
        throw std::runtime_error("Invalid GRID format in " + filepath + ": " +
                                 line);
      }
      continue;
    }

    // Parse LAYOUT header
    if (trimmed.rfind("LAYOUT", 0) == 0) {
      reading_layout = true;
      std::string rest = trimmed.substr(6);
      if (!rest.empty()) {
        for (char& c : rest) {
          if (c == ',') c = ' ';
        }
        std::istringstream iss(rest);
        int val;
        while (iss >> val) {
          puzzle.grid.push_back(val);
        }
      }
      continue;
    }

    // Parse layout numbers
    if (reading_layout) {
      std::string s = line;
      for (char& c : s) {
        if (c == ',') c = ' ';
      }
      std::istringstream iss(s);
      int val;
      while (iss >> val) {
        puzzle.grid.push_back(val);
      }
      continue;
    }
  }

  if (puzzle.width <= 0 || puzzle.height <= 0) {
    throw std::runtime_error("Missing or invalid GRID dimensions in " +
                             filepath);
  }

  if (puzzle.grid.size() !=
      static_cast<size_t>(puzzle.width * puzzle.height)) {
    throw std::runtime_error("Grid size mismatch in " + filepath +
                             ": expected " +
                             std::to_string(puzzle.width * puzzle.height) +
                             ", got " + std::to_string(puzzle.grid.size()));
  }

  // 1. Validate layout numbers:
  // Numbers must be -1 (black cell), 0 (white continuation), or positive clue numbers.
  // Positive clue numbers must be strictly ascending with no repeats.
  int prev_clue_num = 0;
  std::map<int, std::pair<int, int>> clue_coords;

  for (size_t i = 0; i < puzzle.grid.size(); ++i) {
    int val = puzzle.grid[i];
    if (val == 0 || val == -1) {
      continue;
    }
    if (val < -1) {
      throw std::runtime_error("Invalid value in layout at cell " +
                               std::to_string(i) + ": " + std::to_string(val) +
                               " (must be -1, 0, or positive clue number)");
    }
    if (val <= prev_clue_num) {
      if (val == prev_clue_num) {
        throw std::runtime_error("Duplicate clue number in layout: " +
                                 std::to_string(val));
      } else {
        throw std::runtime_error(
            "Clue numbers in layout must be strictly ascending: found " +
            std::to_string(val) + " after " +
            std::to_string(prev_clue_num));
      }
    }
    prev_clue_num = val;
    int x = static_cast<int>(i % puzzle.width);
    int y = static_cast<int>(i / puzzle.width);
    clue_coords[val] = {x, y};
  }

  // 2. Validate clue scripts:
  // Check format: must start with 'a' (across) or 'd' (down) followed by digits.
  // Validate no unused hints: every hint in clue_scripts must exist in layout.
  // Validate hint length: all hints must refer to numbers with two or more digits (no single digit numbers).
  for (const auto& [clue_id, _] : puzzle.clue_scripts) {
    if (clue_id.size() < 2 || (clue_id[0] != 'a' && clue_id[0] != 'd')) {
      throw std::runtime_error(
          "Invalid clue ID '" + clue_id +
          "': must start with 'a' (across) or 'd' (down) followed by a number");
    }
    for (size_t i = 1; i < clue_id.size(); ++i) {
      if (!std::isdigit(static_cast<unsigned char>(clue_id[i]))) {
        throw std::runtime_error(
            "Invalid clue ID '" + clue_id +
            "': clue number contains non-digit characters");
      }
    }
    int clue_num = std::stoi(clue_id.substr(1));
    auto it = clue_coords.find(clue_num);
    if (it == clue_coords.end()) {
      throw std::runtime_error("Unused hint '" + clue_id +
                               "': clue number " + std::to_string(clue_num) +
                               " does not exist in the layout");
    }

    auto [cx, cy] = it->second;
    int hint_len = 0;
    if (clue_id[0] == 'a') {
      for (int c = cx;
           c < puzzle.width && puzzle.grid[cy * puzzle.width + c] != -1; ++c) {
        hint_len++;
      }
    } else {
      for (int r = cy;
           r < puzzle.height && puzzle.grid[r * puzzle.width + cx] != -1; ++r) {
        hint_len++;
      }
    }

    if (hint_len < 2) {
      throw std::runtime_error(
          "Hint '" + clue_id + "' at (" + std::to_string(cx) + ", " +
          std::to_string(cy) + ") refers to a number with " +
          std::to_string(hint_len) +
          " digit(s): all hints must refer to numbers with two or more digits (single digit numbers are not allowed)");
    }
  }

  // 3. Validate layout clue possibilities and match with hints:
  // For every clue number in the layout, determine if a horizontal (across) and/or
  // vertical (down) clue starts at that cell.
  // Both across and down hints must refer to lengths >= 2 (no single digit numbers).
  for (int y = 0; y < puzzle.height; ++y) {
    for (int x = 0; x < puzzle.width; ++x) {
      int val = puzzle.grid[y * puzzle.width + x];
      if (val <= 0) {
        continue;
      }

      // Compute across length starting at this cell
      int across_len = 0;
      for (int c = x;
           c < puzzle.width && puzzle.grid[y * puzzle.width + c] != -1; ++c) {
        across_len++;
      }

      // Compute down length starting at this cell
      int down_len = 0;
      for (int r = y;
           r < puzzle.height && puzzle.grid[r * puzzle.width + x] != -1; ++r) {
        down_len++;
      }

      // Horizontal (across) clue starts here if:
      // (1) Left is grid boundary or black cell (-1), AND
      // (2) Rightward span is >= 2.
      bool is_left_boundary =
          (x == 0 || puzzle.grid[y * puzzle.width + (x - 1)] == -1);
      bool can_across = is_left_boundary && (across_len >= 2);

      // Vertical (down) clue starts here if:
      // (1) Above is grid boundary or black cell (-1), AND
      // (2) Downward span is >= 2.
      bool is_top_boundary =
          (y == 0 || puzzle.grid[(y - 1) * puzzle.width + x] == -1);
      bool can_down = is_top_boundary && (down_len >= 2);

      if (!can_across && !can_down) {
        throw std::runtime_error(
            "Invalid clue number " + std::to_string(val) + " at (" +
            std::to_string(x) + ", " + std::to_string(y) +
            "): cannot start either a horizontal or down clue of length >= 2 (all numbers must have two or more digits)");
      }

      std::string across_id = "a" + std::to_string(val);
      std::string down_id = "d" + std::to_string(val);
      bool has_across_hint =
          puzzle.clue_scripts.find(across_id) != puzzle.clue_scripts.end();
      bool has_down_hint =
          puzzle.clue_scripts.find(down_id) != puzzle.clue_scripts.end();

      if (has_across_hint && across_len < 2) {
        throw std::runtime_error(
            "Horizontal hint '" + across_id + "' at (" + std::to_string(x) +
            ", " + std::to_string(y) + ") has length " +
            std::to_string(across_len) +
            ": all hints must refer to numbers with two or more digits (single digit numbers are not allowed)");
      }
      if (can_across && !has_across_hint) {
        throw std::runtime_error("Missing horizontal hint '" + across_id +
                                 "': layout has an across clue starting at (" +
                                 std::to_string(x) + ", " + std::to_string(y) +
                                 ") with length " + std::to_string(across_len) +
                                 " but no hint script was provided");
      }
      if (!can_across && has_across_hint) {
        throw std::runtime_error(
            "Invalid horizontal hint '" + across_id +
            "': layout does not allow an across clue at clue number " +
            std::to_string(val));
      }

      if (has_down_hint && down_len < 2) {
        throw std::runtime_error(
            "Down hint '" + down_id + "' at (" + std::to_string(x) +
            ", " + std::to_string(y) + ") has length " +
            std::to_string(down_len) +
            ": all hints must refer to numbers with two or more digits (single digit numbers are not allowed)");
      }
      if (can_down && !has_down_hint) {
        throw std::runtime_error("Missing down hint '" + down_id +
                                 "': layout has a down clue starting at (" +
                                 std::to_string(x) + ", " + std::to_string(y) +
                                 ") with length " + std::to_string(down_len) +
                                 " but no hint script was provided");
      }
      if (!can_down && has_down_hint) {
        throw std::runtime_error(
            "Invalid down hint '" + down_id +
            "': layout does not allow a down clue at clue number " +
            std::to_string(val));
      }
    }
  }

  // 4. Validate that no playable cell is an isolated single-digit position:
  // Every non-black cell must belong to at least one entry (across or down) of length >= 2.
  for (int y = 0; y < puzzle.height; ++y) {
    for (int x = 0; x < puzzle.width; ++x) {
      if (puzzle.grid[y * puzzle.width + x] == -1) {
        continue;
      }

      int h_span = 0;
      for (int c = x; c >= 0 && puzzle.grid[y * puzzle.width + c] != -1; --c) {
        h_span++;
      }
      for (int c = x + 1;
           c < puzzle.width && puzzle.grid[y * puzzle.width + c] != -1; ++c) {
        h_span++;
      }

      int v_span = 0;
      for (int r = y; r >= 0 && puzzle.grid[r * puzzle.width + x] != -1; --r) {
        v_span++;
      }
      for (int r = y + 1;
           r < puzzle.height && puzzle.grid[r * puzzle.width + x] != -1; ++r) {
        v_span++;
      }

      if (h_span < 2 && v_span < 2) {
        throw std::runtime_error(
            "Single-digit number position detected at (" + std::to_string(x) +
            ", " + std::to_string(y) +
            "): every cell must belong to an entry of length >= 2 (single digit numbers are not allowed)");
      }
    }
  }

  return puzzle;
}
