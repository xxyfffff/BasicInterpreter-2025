#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      // TODO: The main function.
      // 处理命令行
      if (line == "LIST") {
        program.list();
        continue;
      }
      else if (line == "RUN") {
        program.run();
        continue;
      }
      else if (line == "CLEAR") {
        program.clear();
        continue;
      }
      else if (line == "QUIT") {
        return 0;
      }

      // 处理立即执行语句
      std::string tmp = line.substr(0, 3);
      if (tmp == "LET" || tmp == "PRI" || tmp == "INP") {
        TokenStream tokens = lexer.tokenize(line);
        if (tokens.empty()) {
          throw BasicError("SYNTAX ERROR");
        }
        std::unique_ptr<ParsedLine> parsedLine = parser.parseLine(tokens, line);
        std::unique_ptr<Statement> stmt = parsedLine->fetchStatement();
        program.execute(stmt.get());
        continue;
      }

      // 处理程序行
      // 1.词法分析
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) {
        throw BasicError("SYNTAX ERROR");
      }
      // 2.语法分析
      std::unique_ptr<ParsedLine> parsedLine = parser.parseLine(tokens, line);
      // 3.解释执行
      if (parsedLine->getLine().has_value()) {
        int lineNum = parsedLine->getLine().value();
        std::unique_ptr<Statement> stmt = parsedLine->fetchStatement();
        if (stmt) {
          program.addStmt(lineNum, stmt.release()); // release() 转移所有权给 Program
        } else {
          program.removeStmt(lineNum);
        }
      }
      else {
        throw BasicError("SYNTAX ERROR");
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}