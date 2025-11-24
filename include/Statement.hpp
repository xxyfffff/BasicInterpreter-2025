#pragma once

#include <memory>
#include <string>

#include "Expression.hpp"

class Program;
class VarState;

class Statement {
 public:
  explicit Statement(std::string source);
  virtual ~Statement() = default;

  virtual void execute(VarState& state, Program& program) const = 0;

  const std::string& text() const noexcept;

 private:
  std::string source_;
};

// TODO: Other statement types derived from Statement, e.g., GOTOStatement,
// LetStatement, etc.

class LETStatement : public Statement {
  std::string var;
  std::unique_ptr<Expression> expr;
public:
  LETStatement(std::string source, std::string var, std::unique_ptr<Expression> expr);
  void execute(VarState& state, Program& program) const override;
};

class PRINTStatement : public Statement {
  std::unique_ptr<Expression> expr;
public:
  PRINTStatement(std::string source, std::unique_ptr<Expression> expr);
  void execute(VarState& state, Program& program) const override;
};

class INPUTStatement : public Statement {
  std::string var;
public:
  INPUTStatement(std::string source, std::string var);
  void execute(VarState& state, Program& program) const override;
};

class GOTOStatement : public Statement {
  int line;
public:
  GOTOStatement(std::string source, int line);
  void execute(VarState& state, Program& program) const override;
};

class IFStatement : public Statement {
  std::unique_ptr<Expression> expr1;
  std::unique_ptr<Expression> expr2;
  char op;
  int line;
public:
  IFStatement(std::string source, std::unique_ptr<Expression> expr1,
    std::unique_ptr<Expression> expr2, char op, int line);
  void execute(VarState& state, Program& program) const override;
};

class REMStatement : public Statement {
public:
  REMStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};

class ENDStatement : public Statement {
public:
  ENDStatement(std::string source);
  void execute(VarState& state, Program& program) const override;
};
