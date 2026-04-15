#include "digit.h"

#include <string>

Digit::Digit() { std::fill(digit_store, digit_store + 10, true); }
bool Digit::getDigitValid(int n) { return digit_store[n]; }
void Digit::setDigitValid(int n) { digit_store[n] = true; }
void Digit::setDigitInvalid(int n) { digit_store[n] = false; }
void Digit::clearAll() { std::fill(digit_store, digit_store + 10, false); }
void Digit::print() {}
