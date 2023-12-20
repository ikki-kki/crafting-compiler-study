#include "Main.h"

auto main(int argc, char **argv) -> int
{
  string sourceCode = R""""(
    function main() {
          printLine 'Hello, World!';
      printLine 1 =2 * 3;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  printTokenList(tokenList);
  return 0;
}


void printTokenList(vector<Token> tokenList) {
  for (Token token : tokenList) {
    cout << setw(20) << left << toString(token.kind) << token.string << endl;
  }
}
