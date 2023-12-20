#include "Main.h"

/**
 * 구문 분석기
 * - 소스코드의 구문을 분석하는 프로그램
 * - 토큰 리스트를 입력받아 구문 트리를 출력
*/

auto main(int argc, char** argv)->int {  
  string sourceCode = R""""(
    function main() {
      print 'Hello, World!';
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  printSyntaxTree(syntaxTree);
  return 0; // 책에 없는 내용
}