#include "Program.hpp"

#include <iostream>

#include "utils/Error.hpp"

// TODO: Imply interfaces declared in the Program.hpp.
Program::Program():programCounter_(0),programEnd_(false)
{}

void Program::addStmt(int line, Statement* stmt) {
  if (line <= 0) {
    throw BasicError("SYNTAX ERROR");
  }
  if (stmt == nullptr) {
    throw BasicError("SYNTAX ERROR");
  }
  recorder_.add(line, stmt);
}

void Program::removeStmt(int line) {
  recorder_.remove(line);
}

void Program::run() {
  resetAfterRun();
  programCounter_ = recorder_.nextLine(programCounter_);
  if (programCounter_ != -1) {
    while (!programEnd_) {
      const Statement* curStmt = recorder_.get(programCounter_);
      if (!curStmt) {
        throw BasicError("SYNTAX ERROR");
        break;
      }
      curStmt->execute(vars_, *this);
    }
  }
}

void Program::list() const {
  recorder_.printLines();
}

void Program::clear() {
  recorder_.clear();
  vars_.clear();
}

void Program::execute(Statement* stmt) {
  if (!stmt) {
    return;
  }
  stmt->execute(vars_, *this);
}

int Program::getPC() const noexcept {
  return programCounter_;
}

void Program::changePC(int line) {
  if (line <= 0) {
    throw BasicError("SYNTAX ERROR");
    return;
  }
  if (!recorder_.hasLine(line)) {
    throw BasicError("SYNTAX ERROR");
    return;
  }
  programCounter_ = line;
}

void Program::programEnd() {
  programEnd_ = true;
}

void Program::resetAfterRun() noexcept {
  programCounter_ = -1;
  programEnd_ = false;
}