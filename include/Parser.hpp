#pragma once

#include <memory>
#include <optional>

#include "Token.hpp"

class Statement;
class Expression;

class ParsedLine {
 private:
  std::optional<int> line_number_;
  std::unique_ptr<Statement> statement_;

 public:
  ParsedLine();
  ~ParsedLine();

  void setLine(int line);
  std::optional<int> getLine();
  void setStatement(std::unique_ptr<Statement> stmt);
  const Statement* getStatement() const;
  std::unique_ptr<Statement> fetchStatement();
};

class Parser {
 public:
  std::unique_ptr<ParsedLine> parseLine(TokenStream& tokens,
                       const std::string& originLine) const;

 private:
  std::unique_ptr<Statement> parseStatement(TokenStream& tokens,
                            const std::string& originLine) const;
  std::unique_ptr<Statement> parseLet(TokenStream& tokens, const std::string& originLine) const;
  std::unique_ptr<Statement> parsePrint(TokenStream& tokens,
                        const std::string& originLine) const;
  std::unique_ptr<Statement> parseInput(TokenStream& tokens,
                        const std::string& originLine) const;
  std::unique_ptr<Statement> parseGoto(TokenStream& tokens,
                       const std::string& originLine) const;
  std::unique_ptr<Statement> parseIf(TokenStream& tokens, const std::string& originLine) const;
  std::unique_ptr<Statement> parseRem(TokenStream& tokens, const std::string& originLine) const;
  std::unique_ptr<Statement> parseEnd(TokenStream& tokens, const std::string& originLine) const;

  std::unique_ptr<Expression> parseExpression(TokenStream& tokens) const;
  std::unique_ptr<Expression> parseExpression(TokenStream& tokens, int precedence) const;

  int getPrecedence(TokenType op) const;
  int parseLiteral(const Token* token) const;

  mutable int leftParentCount{0};
};
