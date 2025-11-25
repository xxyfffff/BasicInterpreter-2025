#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept {
  static std::string txt;
  txt.clear();

  size_t i = 0;
  while (i < source_.size() && std::isdigit(source_[i])) {
    ++i;
  }
  while (i < source_.size() && std::isspace(source_[i])) {
    ++i;
  }

  txt = source_.substr(i);

  return txt;
}

// TODO: Imply interfaces declared in the Statement.hpp.
LETStatement::LETStatement(std::string source,
    std::string var,
    std::unique_ptr<Expression> expr):
  Statement(std::move(source)),
  var(std::move(var)),
  expr(std::move(expr))// 只能move，转移所有权
  {}

void LETStatement::execute(VarState& state, Program& program) const {
  int value = expr->evaluate(state);
  state.setValue(var, value);
}

PRINTStatement::PRINTStatement(std::string source,
    std::unique_ptr<Expression> expr):
  Statement(std::move(source)),
  expr(std::move(expr))// 只能move，转移所有权
  {}

void PRINTStatement::execute(VarState& state, Program& program) const {
  int value = expr->evaluate(state);
  std::cout << value << std::endl;
}

INPUTStatement::INPUTStatement(std::string source,
    std::string var):
  Statement(std::move(source)),
  var(std::move(var))
  {}

void INPUTStatement::execute(VarState& state, Program& program) const {
  bool valid = false;
  while (!valid) {
    std::string input;
    std::cout << ' ' << '?' << ' ';
    std::getline(std::cin,input);
    bool flag = true;
    int sign = 1;
    int value = 0;
    if (input[0] == '-') {
      sign = -1;
      input = input.substr(1);
    }
    for (auto c : input) {
      if (c >= '0' && c <= '9') {
        value = value * 10 + c - '0';
        continue;
      }
      else {
        flag = false;
      }
    }
    if (flag) {
      state.setValue(var, value * sign);
      valid = true;
    }
    else {
      std::cout << "INVALID NUMBER" << std::endl;
    }
  }
}

GOTOStatement::GOTOStatement(std::string source,
    int line):
  Statement(std::move(source)),
  line(std::move(line))
{}

void GOTOStatement::execute(VarState& state, Program& program) const {
  program.changePC(line);
}

IFStatement::IFStatement(std::string source,
    std::unique_ptr<Expression> expr1,
    std::unique_ptr<Expression> expr2,
    char op,
    int line):
  Statement(std::move(source)),
  expr1(std::move(expr1)),
  expr2(std::move(expr2)),
  op(op),
  line(line)
{}

void IFStatement::execute(VarState& state, Program& program) const {
  int val1 = expr1->evaluate(state);
  int val2 = expr2->evaluate(state);
  bool flag = false;
  switch (op) {
    case '=':
      flag = (val1 == val2);
      break;
    case '>':
      flag = (val1 > val2);
      break;
    case '<':
      flag = (val1 < val2);
      break;
    default:
      throw BasicError("SYNTAX ERROR");
  }
  if (flag) {
    program.changePC(line);
  }
}

REMStatement::REMStatement(std::string source):
  Statement(std::move(source))
{}

void REMStatement::execute(VarState& state, Program& program) const {
  return;
}

ENDStatement::ENDStatement(std::string source):
  Statement(std::move(source))
{}

void ENDStatement::execute(VarState& state, Program& program) const {
  program.programEnd();
}