#include "Parser.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Expression.hpp"
#include "Statement.hpp"
#include "utils/Error.hpp"

ParsedLine::ParsedLine() { statement_ = nullptr; }

ParsedLine::~ParsedLine() {}

void ParsedLine::setLine(int line) { line_number_.emplace(line); }

std::optional<int> ParsedLine::getLine() { return line_number_; }

void ParsedLine::setStatement(std::unique_ptr<Statement> stmt) { statement_ = std::move(stmt); }

const Statement* ParsedLine::getStatement() const { return statement_.get(); }

std::unique_ptr<Statement> ParsedLine::fetchStatement() {
  std::unique_ptr<Statement> temp = std::move(statement_);
  statement_ = nullptr;
  return temp;
}

std::unique_ptr<ParsedLine> Parser::parseLine(TokenStream& tokens,
                             const std::string& originLine) const {
  auto result = std::make_unique<ParsedLine>();

  // 检查是否有行号
  const Token* firstToken = tokens.peek();
  if (firstToken && firstToken->type == TokenType::NUMBER) {
    // 解析行号
    result->setLine(parseLiteral(firstToken));
    tokens.get();  // 消费行号token

    // 如果只有行号，表示删除该行
    if (tokens.empty()) {
      return std::move(result);
    }
  }

  // 解析语句
  result->setStatement(parseStatement(tokens, originLine));

  return std::move(result);
}

std::unique_ptr<Statement> Parser::parseStatement(TokenStream& tokens,
                                  const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  switch (token->type) {
    case TokenType::LET:
      return parseLet(tokens, originLine);
    case TokenType::PRINT:
      return parsePrint(tokens, originLine);
    case TokenType::INPUT:
      return parseInput(tokens, originLine);
    case TokenType::GOTO:
      return parseGoto(tokens, originLine);
    case TokenType::IF:
      return parseIf(tokens, originLine);
    case TokenType::REM:
      return parseRem(tokens, originLine);
    case TokenType::END:
      return parseEnd(tokens, originLine);
    default:
      throw BasicError("SYNTAX ERROR");
  }
}

std::unique_ptr<Statement> Parser::parseLet(TokenStream& tokens,
                            const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;

  if (tokens.empty() || tokens.get()->type != TokenType::EQUAL) {
    throw BasicError("SYNTAX ERROR");
  }

  auto expr = parseExpression(tokens);
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<LETStatement>(originLine, varName,
    std::move(expr));
}

std::unique_ptr<Statement> Parser::parsePrint(TokenStream& tokens,
                              const std::string& originLine) const {
  auto expr = parseExpression(tokens);
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<PRINTStatement>(originLine, std::move(expr));
}

std::unique_ptr<Statement> Parser::parseInput(TokenStream& tokens,
                              const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* varToken = tokens.get();
  if (!varToken || varToken->type != TokenType::IDENTIFIER) {
    throw BasicError("SYNTAX ERROR");
  }

  std::string varName = varToken->text;
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<INPUTStatement>(originLine, varName);
}

std::unique_ptr<Statement> Parser::parseGoto(TokenStream& tokens,
                             const std::string& originLine) const {
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<GOTOStatement>(originLine, targetLine);
}

std::unique_ptr<Statement> Parser::parseIf(TokenStream& tokens,
                           const std::string& originLine) const {
  // 解析左表达式
  auto leftExpr = parseExpression(tokens);

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  // 解析比较操作符
  const Token* opToken = tokens.get();
  char op;
  switch (opToken->type) {
    case TokenType::EQUAL:
      op = '=';
      break;
    case TokenType::GREATER:
      op = '>';
      break;
    case TokenType::LESS:
      op = '<';
      break;
    default:
      throw BasicError("SYNTAX ERROR");
  }

  // 解析右表达式
  auto rightExpr = parseExpression(tokens);

  // 检查THEN关键字
  if (tokens.empty() || tokens.get()->type != TokenType::THEN) {
    throw BasicError("SYNTAX ERROR");
  }

  // 解析目标行号
  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* lineToken = tokens.get();
  if (!lineToken || lineToken->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  int targetLine = parseLiteral(lineToken);

  // TODO: create a corresponding stmt and return it.
  return std::make_unique<IFStatement>(originLine, std::move(leftExpr),
    std::move(rightExpr), op, targetLine);
}

std::unique_ptr<Statement> Parser::parseRem(TokenStream& tokens,
                            const std::string& originLine) const {
  const Token* remInfo = tokens.get();
  if (!remInfo || remInfo->type != TokenType::REMINFO) {
    throw BasicError("SYNTAX ERROR");
  }
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<REMStatement>(originLine);
}

std::unique_ptr<Statement> Parser::parseEnd(TokenStream& tokens,
                            const std::string& originLine) const {
  // TODO: create a corresponding stmt and return it.
  return std::make_unique<ENDStatement>(originLine);
}

std::unique_ptr<Expression> Parser::parseExpression(TokenStream& tokens) const {
  return parseExpression(tokens, 0);
}

std::unique_ptr<Expression> Parser::parseExpression(TokenStream& tokens, int precedence) const {
  // 解析左操作数
  std::unique_ptr<Expression> left;

  if (tokens.empty()) {
    throw BasicError("SYNTAX ERROR");
  }

  const Token* token = tokens.get();
  if (!token) {
    throw BasicError("SYNTAX ERROR");
  }

  if (token->type == TokenType::NUMBER) {
    int value = parseLiteral(token);
    left = std::make_unique<ConstExpression>(value);
  } else if (token->type == TokenType::IDENTIFIER) {
    left = std::make_unique<VariableExpression>(token->text);
  } else if (token->type == TokenType::LEFT_PAREN) {
    ++leftParentCount;
    left = parseExpression(tokens, 0);

    if (tokens.empty() || tokens.get()->type != TokenType::RIGHT_PAREN) {
      throw BasicError("MISMATCHED PARENTHESIS");
    }
    --leftParentCount;
  } else {
    throw BasicError("SYNTAX ERROR");
  }

  // 检查是否有运算符
  while (!tokens.empty()) {
    const Token* opToken = tokens.peek();
    if (!opToken) {
      break;
    }

    // 检查是否是右括号
    if (opToken->type == TokenType::RIGHT_PAREN) {
      if (leftParentCount == 0) {
        throw BasicError("MISMATCHED PARENTHESIS");
      }
      break;
    }

    // 检查是否是运算符
    int opPrecedence = getPrecedence(opToken->type);
    if (opPrecedence == -1 || opPrecedence < precedence) {
      break;
    }

    tokens.get();  // 消费运算符token

    char op;
    switch (opToken->type) {
      case TokenType::PLUS:
        op = '+';
        break;
      case TokenType::MINUS:
        op = '-';
        break;
      case TokenType::MUL:
        op = '*';
        break;
      case TokenType::DIV:
        op = '/';
        break;
      default:
        throw BasicError("SYNTAX ERROR");
    }

    // 解析右操作数，使用更高的优先级
    auto right = parseExpression(tokens, opPrecedence + 1);
    left = std::make_unique<CompoundExpression>(std::move(left),
     op, std::move(right));
  }

  return left;
}

int Parser::getPrecedence(TokenType op) const {
  switch (op) {
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 1;
    case TokenType::MUL:
    case TokenType::DIV:
      return 2;
    default:
      return -1;
  }
}

int Parser::parseLiteral(const Token* token) const {
  if (!token || token->type != TokenType::NUMBER) {
    throw BasicError("SYNTAX ERROR");
  }

  try {
    size_t pos;
    int value = std::stoi(token->text, &pos);

    // 检查是否整个字符串都被解析
    if (pos != token->text.length()) {
      throw BasicError("INT LITERAL OVERFLOW");
    }

    return value;
  } catch (const std::out_of_range&) {
    throw BasicError("INT LITERAL OVERFLOW");
  } catch (const std::invalid_argument&) {
    throw BasicError("SYNTAX ERROR");
  }
}