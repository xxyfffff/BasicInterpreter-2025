// TODO: Imply interfaces declared in the Recorder.hpp.

#include "Recorder.hpp"

#include <iostream>

#include "utils/Error.hpp"

Recorder::~Recorder() {
  for (auto &[line, stmt] : lineToStmt) {
    delete stmt;
  }
  lineToStmt.clear();
}

void Recorder::add(int line, Statement* stmt) {
  if (line <= 0) {
    throw BasicError("SYNTAX ERROR");
  }
  if (!stmt) {
    throw BasicError("SYNTAX ERROR");
  }

  auto it = lineToStmt.find(line);
  if (it != lineToStmt.end()) {
    delete it->second;
    lineToStmt.erase(it);
  }
  lineToStmt.emplace(line, stmt);
}

void Recorder::remove(int line) {
  auto it = lineToStmt.find(line);
  if (it != lineToStmt.end()) {
    delete it->second;
    lineToStmt.erase(it);
  }
}

const Statement* Recorder::get(int line) const noexcept {
  auto it = lineToStmt.find(line);
  return (it != lineToStmt.end()) ? it->second : nullptr;
}

bool Recorder::hasLine(int line) const noexcept {
  return lineToStmt.count(line) > 0;
}

void Recorder::clear() noexcept {
  for (auto& [line, stmt] : lineToStmt) {
    delete stmt;
  }
  lineToStmt.clear();
}

void Recorder::printLines() const {
  for (auto& [line, stmt] : lineToStmt) {
    std::cout << line << ": " << stmt->text() << std::endl;
  }
}

int Recorder::nextLine(int line) const noexcept {
  auto it = lineToStmt.upper_bound(line);
  return (it != lineToStmt.end()) ? it->first : -1;
}