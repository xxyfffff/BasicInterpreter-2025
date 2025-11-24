#include "Expression.hpp"

#include "VarState.hpp"
#include "utils/Error.hpp"

ConstExpression::ConstExpression(int value) : value_(value) {}

int ConstExpression::evaluate(const VarState&) const { return value_; }

VariableExpression::VariableExpression(std::string name)
    : name_(std::move(name)) {}

int VariableExpression::evaluate(const VarState& state) const {
  return state.getValue(name_);
}

CompoundExpression::CompoundExpression(std::unique_ptr<Expression> left, char op,
                                       std::unique_ptr<Expression> right)
    : left_(std::move(left)), right_(std::move(right)), op_(op) {}

CompoundExpression::~CompoundExpression() {
}

int CompoundExpression::evaluate(const VarState& state) const {
  int lhs = left_->evaluate(state);
  int rhs = right_->evaluate(state);

  switch (op_) {
    case '+':
      return lhs + rhs;
    case '-':
      return lhs - rhs;
    case '*':
      return lhs * rhs;
    case '/':
      if (rhs == 0) {
        throw BasicError("DIVIDE BY ZERO");
      }
      return lhs / rhs;
    default:
      throw BasicError("UNSUPPORTED OPERATOR");
  }
}
