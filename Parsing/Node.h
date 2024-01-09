#pragma once
#include <vector>
#include <string>
#include "Token.h"
#include <map>
/**
 * 참고 개념
 * - 구문 분석은 소스 코드의 구조(문과 식)를 분석하는 과정이다. 소스 코드의 구조는 '문'과 '식'으로 구성된다
 *   문은 문을 포함할 수 있다 | 문은 식을 포함할 수 있다 | 식은 식을 포함할 수 있다
 *   복합문 : for, if 처럼 다른 문을 포함할 수 있음
 *   단일문 : return, continue 처럼 다른 문을 포함할 수 없음
 *   식: 1+2*4 같은 것, 연산자의 우선순위에 따라 먼저 계산해야 할 것이 달라진다
 * 
 * - C++에서 vector는 크기가 자동으로 조절되는 배열을 뜻함
 * - struct는 사용자 정의 구조체임
 * - C++에서 별표(*)가 변수 타입 앞에 붙으면, 그 변수는 포인터임을 나타냄. 포인터는 메모리 주소를 저장하는 변수로, 그 주소에 위치한 데이터에 접근할 수 있게 해줌
*/

using std::map;
using std::string;
using std::vector;

// 구문 트리의 루트 노드이자 소스코드 선언 영역을 표현하는 노드
// 함수 노드의 리스트를 멤버로 가짐.
struct Program {
  vector<struct Function*> functions;
};

/**
 * 이후에 정의하는 노드들은 문이나 식에 따라 다음 노드 중 하나를 상속받음
*/
// 모든 문 노드
struct Statement {
  virtual auto print(int) -> void = 0;
};
// 식 노드의 부모 노드
struct Expression {
  virtual auto print(int) -> void = 0;
};


// 문을 상속받는 노드들 (함수)
struct Function : Statement
{
  // 함수 이름
  string name;
  // 함수의 파라미터들
  vector<string> parameters;
  // 함수 본문 (문으로 이루어진 벡터)
  vector<Statement *> block;
  auto print(int) -> void;
};

// return 문을 표현하고 반환식을 멤버로 가지는 노드
struct Return: Statement {
  Expression* expression;
  auto print(int)->void;
};

// 변수의 선언을 표현하는 노드
struct Variable: Statement { 
  string name;
  Expression* expression;
  auto print(int)->void;
};

// for문을 표현하는 노드
struct For: Statement {
  // 변수 선언
  Variable* variable;
  // 조건식
  Expression* condition;
  // 증감식
  Expression* expression;
  // 실행할 문 리스트
  vector<Statement*> block;
  auto print(int)->void;
};

struct Break: Statement {
  auto print(int)->void;
};
struct Continue: Statement {
  auto print(int)->void;
};

// if문을 표현하는 노드
struct If: Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
  auto print(int)->void;
};

struct Print: Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
  auto print(int)->void;
};

// ---

/**
 * 식의 문을 표헌하는 노드
 * 식의 문이란? 소비되지 않는 식의 결과값을 임의로 소비시키기 위해 식을 감싸는 문 노드
*/
struct ExpressionStatement: Statement {
  Expression* expression;
  auto print(int)->void;
};


/**
 * 네 개의 이상 연산자 노드들이 하나의 노드로 표현되지 않는 이유.
 * 1. 논리 연산자 AND와 OR를 구분한 이유는 단락평가 떄문이다
 * 2. AND와 OR는 때에 따라서 왼쪽 식만 평가하므로, 항상 양쪽 식을 모두 평가해야 하는 관계 연산자나 산술 연산자와는 다르다
*/

// OR 연산자 || 
struct Or: Expression {
  Expression* lhs; // x
  Expression* rhs; // y
  auto print(int)->void;
};

// And 연산자 &&
struct And: Expression {
  Expression* lhs; 
  Expression* rhs;
  auto print(int)->void;
};

// 관계 연산자
// <, >, <=, >=, ==,
struct Relational : Expression
{
  Kind kind;      
  Expression *lhs; 
  Expression *rhs; 
  auto print(int)->void;
};

// 산술 연산자
// +, =, *, /, %
struct Arithmetic : Expression
{
  Kind kind;       // +, -, *, /, %
  Expression *lhs; // x
  Expression *rhs; // y
  auto print(int) -> void;
};

// ---

// 단항 연산자 
// +, -
struct Unary: Expression {
  Kind kind;
  Expression* sub;
  auto print(int)->void;
};

// 함수 호출식
// f(x, y, z)
struct Call: Expression {
  Expression* sub;
  vector<Expression*> arguments;  
  auto print(int)->void;
};

// 참조식
// array[0], object["key"]
struct GetElement : Expression
{
  Expression *sub;   // array, object
  Expression *index; // 0, "key"
  auto print(int) -> void;
};

// 수정식
// array[0] = 3; map['property'] = 3;
struct SetElement : Expression
{
  Expression *sub;   
  Expression *index; 
  Expression *value;
  auto print(int)->void;
};

// 변수 참조식
// x
struct GetVariable: Expression {
  string name;
  auto print(int)->void;
};

// 변수 수정식
// x = 3
struct SetVariable: Expression {
  string name;
  Expression* value;
  auto print(int)->void;
};

// --- 

/**
 * 리터럴
 * */ 

// null 리터럴
struct NullLiteral: Expression {
  auto print(int)->void;
};

// 불리언 리터럴
struct BooleanLiteral: Expression {
  bool value = false;
  auto print(int)->void;
};

// 숫자 리터럴
struct NumberLiteral: Expression {
  double value = 0.0;
  auto print(int)->void;
};

// 문자열 리터럴
struct StringLiteral: Expression {
  string value;
  auto print(int)->void;
};

// 배열 리터럴
struct ArrayLiteral : Expression {
  vector<Expression *> values; 
  auto print(int) -> void;
};

// 맵 리터럴
// {'a', 1, 'b', 2}
struct MapLiteral : Expression
{
  map<string, Expression *> values; // x: 10, y: 20
  auto print(int) -> void;
};