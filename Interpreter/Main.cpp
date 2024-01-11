#include "Main.h"

// auto main(int argc, char** argv)->int {  
//   string sourceCode = R""""(
//     function main() {
//       print 'Hello, World!';
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   // 싪행 결과를 출력하는 인터프리터 정의
//   interpret(syntaxTree);
//   return 0; // 책에 없는 내용
// }

// ?NOTE: 산술 연산 테스트용 예제
// auto main(int argc, char** argv) -> int {
//   string sourceCode = R""""(
//     function main() {
//       var a = 10;
//       var b = 5;
//       printLine a + b; 
//       printLine a - b; 
//       printLine a * b; 
//       printLine a / b; 
//       printLine a % b; 
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 논리 연산 테스트용 예제
// auto main(int argc, char** argv) -> int {
//   string sourceCode = R""""(
//     function main() {
//       printLine true or 'Yongseok Club'; 
//       printLine false or 'Yongseok Club'; 
//       printLine true and 'Yongseok Club'; 
//       printLine false and 'Yongseok Club'; 
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 지역 변수 전역 변수 선언 참조 테스트용 예제
// auto main(int argc, char** argv) -> int {
// string sourceCode = R""""(
//     function main() {
//       global = 4;
//       var local = 13;
//       printLine 'global: ', global; 
//       printLine 'local: ', local; 
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 지역 변수 전역 변수 선언 참조 테스트용 예제
// auto main(int argc, char** argv) -> int {
//   string sourceCode = R""""(
//     function main() {
//       global = 4;
//       var local = 13;
//       global = local = 7;
//       printLine 'global: ', global;
//       printLine 'local: ', local;
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }


// ?NOTE: For문 테스트용 예제
// auto main(int argc, char** argv) -> int {
// string sourceCode = R""""(
//     function main() {
//       for i=0, i<3, i=i+1 {
//         if i == 1{
//           continue;
//         }
//         printLine 'i: ', i; 
//       }
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: If문 테스트용 예제
// auto main(int argc, char** argv) -> int {
// string sourceCode = R""""(
//     function main() {
//       for i=0, i<5, i=i+1 {
//         if i == 1 {
//           printLine 'one';
//         } elif i == 2 {
//           printLine 'two';
//         } else {
//           printLine i;
//         }
//       }
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 함수 호출문 테스트용 예제
auto main(int argc, char** argv) -> int {
string sourceCode = R""""(
    function main() {
      print getC(3, 4);
    }

    function getC(a, b) {
      return a * a + b * b;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  return 0; 
}

// ?NOTE: 배열 테스트용 예제
// auto main(int argc, char** argv) -> int {
// string sourceCode = R""""(
//     function main() {
//       var array = ['Jang', 'Yong', 'Seok'];
//       print array; 
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 원소값 참조 테스트용 예제
// auto main(int argc, char** argv) -> int {
// string sourceCode = R""""(
//     function main() {
//       print ['Yongseok', 'Nim', 'Genius'][2];
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }

// ?NOTE: 원소값 수정 테스트용 예제
// auto main(int argc, char** argv) -> int {
//   string sourceCode = R""""(
//     function main() {
//       var array = ['Yongseok', 'Nim', 'Genius'];
//       array[2] = 'Babo';
//       print array[2]; 
//     }
//   )"""";
//   auto tokenList = scan(sourceCode);
//   auto syntaxTree = parse(tokenList);
//   interpret(syntaxTree);

//   return 0; 
// }