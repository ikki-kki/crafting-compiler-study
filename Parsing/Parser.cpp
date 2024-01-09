#include "Parser.h"

static auto parseFunction() -> Function *;

static auto parseBlock() -> vector<Statement *>;
static auto parseExpressionStatement() -> ExpressionStatement *;
static auto parseVariable() -> Variable *;

static auto parseFor() -> For *;
static auto parseIf() -> If *;
static auto parsePrint() -> Print *;
static auto parseReturn() -> Return *;
static auto parseBreak() -> Break *;
static auto parseContinue() -> Continue *;

static auto parseExpressionStatement() -> ExpressionStatement *;
static auto parseExpression() -> Expression *;
static auto parseAssignment() -> Expression *;
static auto parseOr() -> Expression *;
static auto parseAnd() -> Expression *;
static auto parseRelational() -> Expression *;
static auto parseAdditive() -> Expression *;
static auto parseMultiplicative() -> Expression *;
static auto parseUnary() -> Expression *;
static auto parseOperand() -> Expression *;
static auto parseNullLiteral() -> Expression *;
static auto parseBooleanLiteral() -> Expression *;
static auto parseNumberLiteral() -> Expression *;
static auto parseStringLiteral() -> Expression *;
static auto parseListLiteral() -> Expression *;
static auto parseMapLiteral() -> Expression *;
static auto parseIdentifier() -> Expression *;
static auto parseInnerExpression() -> Expression *;
static auto parsePostfix(Expression *) -> Expression *;
static auto parseCall(Expression *) -> Expression *;
static auto parseElement(Expression *) -> Expression *;

static auto skipCurrent() -> void;
static auto skipCurrent(Kind) -> void;
static auto skipCurrentIf(Kind) -> bool;
static vector<Token>::iterator current;


// var x = 10;
auto parseVariable() -> Variable * // 변수 선언문 노드를 파싱하는 함수
{
  auto result = new Variable();           // 변수 선언문 노드 생성
  skipCurrent(Kind::Variable);            // current가 variable 토큰인지 확인
  result->name = current->string;         // 변수 이름 설정
  skipCurrent(Kind::Identifier);          // current가 identifier(x) 토큰인지 확인
  skipCurrent(Kind::Assignment);          // current가 = 토큰인지 확인
  result->expression = parseExpression(); // 변수의 표현식을 파싱
  if (result->expression == nullptr)      // 변수의 표현식이 없으면 var x;와 같은 구문이므로
  {
    cout << "변수 선언에 초기화식이 없습니다."; // 에러 메시지 출력
    exit(1);                                    // 프로그램 종료
  }
  skipCurrent(Kind::Semicolon); // current가 ; 토큰인지 확인
  return result;                // 변수 선언문 노드 반환
}

// return x;
auto parseReturn() -> Return *
{
  auto result = new Return();             // return문 노드 생성
  skipCurrent(Kind::Return);              // current가 return 토큰인지 확인
  result->expression = parseExpression(); // 식 노드를 파싱하여 return문 노드의 식으로 설정
  if (result->expression == nullptr)      // 식이 없으면 return문에 식이 없다는 에러 메시지 출력
  {
    cout << "return문에 식이 없습니다.";
    exit(1);
  }
  skipCurrent(Kind::Semicolon); // current가 ; 토큰인지 확인
  return result;
}

// break;
auto parseBreak() -> Break *
{
  auto result = new Break();
  skipCurrent(Kind::Break);
  skipCurrent(Kind::Semicolon);
  return result;
}

// continue;
auto parseContinue() -> Continue *
{
  auto result = new Continue();
  skipCurrent(Kind::Continue);
  skipCurrent(Kind::Semicolon);
  return result;
}

// 논리 or 연산자
auto parseOr() -> Expression * {
  auto result = parseAnd();
  while (skipCurrentIf(Kind::LogicalOr)){
    auto temp = new Or();
    temp-> lhs = result;
    temp-> rhs = parseAnd();
    result = temp;
  }
  return result;
}

// var x = 1 && 2;
auto parseAnd() -> Expression *
{
  auto result = parseRelational(); // 우선순위가 높은 관계 연산자 노드를 파싱
  while (skipCurrentIf(Kind::LogicalAnd))
  {
    auto temp = new And();         // AND 연산자 노드 생성
    temp->lhs = result;            // AND 연산자 노드의 lhs에 관계 연산자 노드를 설정
    temp->rhs = parseRelational(); // AND 연산자 노드의 rhs에 관계 연산자 노드를 설정
    result = temp;                 // 관계 연산자 노드 대신 AND 연산자 노드를 반환
  }
  return result;
}

// var x = 1 < 2;
auto parseRelational() -> Expression *
{
  set<Kind> operators = {
      Kind::Equal,
      Kind::NotEqual,
      Kind::LessThan,
      Kind::GreaterThan,
  };
  auto result = parseAdditive(); // 우선순위가 높은 산술 연산자 노드를 파싱
  while (operators.count(current->kind))
  {
    auto temp = new Relational();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseAdditive();
    result = temp;
  }
  return result;
}

// var x = 1 + 2;
auto parseAdditive() -> Expression *
{
  set<Kind> operators = {
      Kind::Add,
      Kind::Subtract,
  };
  auto result = parseMultiplicative(); // 우선순위가 높은 산술 연산자 노드를 파싱
  while (operators.count(current->kind))
  {
    auto temp = new Arithmetic();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseMultiplicative();
    result = temp;
  }
  return result;
}

// var x = 1 * 2;
auto parseMultiplicative() -> Expression *
{
  set<Kind> operators = {
      Kind::Multiply,
      Kind::Divide,
      Kind::Modulo,
  };
  auto result = parseUnary(); // 우선순위가 높은 단항 연산자 노드를 파싱
  while (operators.count(current->kind))
  {
    auto temp = new Arithmetic();
    temp->kind = current->kind;
    skipCurrent();
    temp->lhs = result;
    temp->rhs = parseUnary();
    result = temp;
  }
  return result;
}


// var x = -1;
auto parseUnary() -> Expression *
{
  set<Kind> operators = {
      Kind::Add,
      Kind::Subtract,
  };
  if (operators.count(current->kind))
  {
    auto result = new Unary();
    result->kind = current->kind;
    skipCurrent();
    result->sub = parseUnary();
    return result;
  }
  return parseOperand();
}

// 피연산자 노드를 파싱하는 함수
auto parseOperand() -> Expression *
{
  Expression *result = nullptr; // 피연산자 노드 포인터
  switch (current->kind)
  {
  case Kind::NullLiteral:
    result = parseNullLiteral(); // null 리터럴 노드를 파싱
    break;
  case Kind::TrueLiteral:
  case Kind::FalseLiteral:
    result = parseBooleanLiteral(); // boolean 리터럴 노드를 파싱
    break;
  case Kind::NumberLiteral:
    result = parseNumberLiteral(); // number 리터럴 노드를 파싱
    break;
  case Kind::StringLiteral:
    result = parseStringLiteral(); // string 리터럴 노드를 파싱
    break;
  case Kind::LeftBracket:
    result = parseListLiteral(); // list 리터럴 노드를 파싱
    break;
  case Kind::LeftBrace:
    result = parseMapLiteral(); // map 리터럴 노드를 파싱
    break;
  case Kind::Identifier:
    result = parseIdentifier(); // 변수 이름 노드를 파싱
    break;
  case Kind::LeftParen:
    result = parseInnerExpression(); // 괄호로 둘러싸인 식 노드를 파싱
    break;
  default:
    cout << "잘못된 식입니다.";
    exit(1);
  }
  return parsePostfix(result); // 후위 연산자 노드를 파싱
}


// NULL 리터럴 노드를 파싱하는 함수
auto parseNullLiteral() -> Expression *
{
  skipCurrent(Kind::NullLiteral);  // current가 null 토큰인지 확인
  auto result = new NullLiteral(); // NULL 리터럴 노드 생성
  return result;                   // NULL 리터럴 노드 반환
}

// boolean 리터럴 노드를 파싱하는 함수
auto parseBooleanLiteral() -> Expression *
{
  auto result = new BooleanLiteral();                 // boolean 리터럴 노드 생성
  result->value = current->kind == Kind::TrueLiteral; // current가 true 토큰이면 true, false 토큰이면 false로 설정
  skipCurrent();                                      //
  return result;                                      // boolean 리터럴 노드 반환
}

// number 리터럴 노드를 파싱하는 함수
auto parseNumberLiteral() -> Expression *
{
  auto result = new NumberLiteral();     // number 리터럴 노드 생성
  result->value = stod(current->string); // current의 문자열을 double로 변환하여 number 리터럴 노드의 값으로 설정
  skipCurrent();                         //
  return result;                         // number 리터럴 노드 반환
}

// string 리터럴 노드를 파싱하는 함수
auto parseStringLiteral() -> Expression *
{
  auto result = new StringLiteral(); // string 리터럴 노드 생성
  result->value = current->string;   // current의 문자열을 string으로 변환하여 string 리터럴 노드의 값으로 설정
  skipCurrent();                     //
  return result;                     // string 리터럴 노드 반환
}

// list 리터럴 노드를 파싱하는 함수
auto parseListLiteral() -> Expression *
{
  auto result = new ArrayLiteral(); // Array 리터럴 노드 생성
  skipCurrent(Kind::LeftBracket);   // current가 [ 토큰인지 확인
  if (current->kind != Kind::RightBracket)
  {
    do
      result->values.push_back(parseExpression()); // current가 ] 토큰이 아닐 때까지 식 노드를 파싱하여 Array 리터럴 노드의 값으로 설정
    while (skipCurrentIf(Kind::Comma));            // current가 , 토큰이면 다음 식 노드로 넘어감. 아니면 반복문 종료
  }
  skipCurrent(Kind::RightBracket); // current가 ] 토큰인지 확인
  return result;                   // Array 리터럴 노드 반환
}

// map 리터럴 노드를 파싱하는 함수
auto parseMapLiteral() -> Expression *
{
  auto result = new MapLiteral(); // Map 리터럴 노드 생성
  skipCurrent(Kind::LeftBrace);   // current가 { 토큰인지 확인
  if (current->kind != Kind::RightBrace)
  {
    do
    {
      auto key = current->string; // current의 문자열을 string으로 변환하여 key로 설정
      skipCurrent(Kind::StringLiteral);
      skipCurrent(Kind::Colon);           // current가 : 토큰인지 확인
      auto value = parseExpression();     // 식 노드를 파싱하여 value로 설정
      result->values[key] = value;        // key와 value를 Map 리터럴 노드의 값으로 설정
    } while (skipCurrentIf(Kind::Comma)); // current가 , 토큰이면 다음 key-value 쌍으로 넘어감. 아니면 반복문 종료
  }
  skipCurrent(Kind::RightBrace); // current가 } 토큰인지 확인
  return result;                 // Map 리터럴 노드 반환
}

// 변수 이름 노드를 파싱하는 함수
// x = 10;
auto parseIdentifier() -> Expression *
{
  auto result = new GetVariable(); // 변수 이름 노드 생성
  result->name = current->string;  // current의 문자열을 string으로 변환하여 변수 이름 노드의 이름으로 설정
  skipCurrent(Kind::Identifier);   // current가 identifier(x) 토큰인지 확인
  return result;                   // 변수 이름 노드 반환
}


// 괄호로 둘러싸인 식 노드를 파싱하는 함수
// (1 + 2) * 3;
auto parseInnerExpression() -> Expression *
{
  skipCurrent(Kind::LeftParen); // current가 ( 토큰인지 확인
  auto result = parseExpression();
  skipCurrent(Kind::RightParen); // current가 ) 토큰인지 확인
  return result;
}

// 후위 연산자 노드를 파싱하는 함수
// x++;
auto parsePostfix(Expression *sub) -> Expression *
{
  while (true) // 반복
  {
    switch (current->kind) // current의 종류에 따라
    {
    case Kind::LeftParen:
      sub = parseCall(sub);
      break; // current가 ( 토큰이면 함수 호출 노드를 파싱
    case Kind::LeftBracket:
      sub = parseElement(sub);
      break; // current가 [ 토큰이면 요소 접근 노드를 파싱
    default:
      return sub; // current가 ( 토큰도 [ 토큰도 아니면 sub 반환
    }
  }
}


// 함수 호출 노드를 파싱하는 함수
// print('Hello, World!');
auto parseCall(Expression *sub) -> Expression *
{
  auto result = new Call();     // 함수 호출 노드 생성
  result->sub = sub;            // 함수 호출 노드의 sub에 식 노드를 설정
  skipCurrent(Kind::LeftParen); // current가 ( 토큰인지 확인
  if (current->kind != Kind::RightParen)
  {
    do
      result->arguments.push_back(parseExpression()); // current가 ) 토큰이 아닐 때까지 식 노드를 파싱하여 함수 호출 노드의 인수로 설정
    while (skipCurrentIf(Kind::Comma));               // current가 , 토큰이면 다음 식 노드로 넘어감. 아니면 반복문 종료
  }
  skipCurrent(Kind::RightParen); // current가 ) 토큰인지 확인
  return result;                 // 함수 호출 노드 반환
}

// 요소 접근 노드를 파싱하는 함수
// x[0];
auto parseElement(Expression *sub) -> Expression *
{
  auto result = new GetElement(); // 요소 접근 노드 생성
  result->sub = sub;              // 요소 접근 노드의 sub에 식 노드를 설정
  skipCurrent(Kind::LeftBracket); // current가 [ 토큰인지 확인
  result->index = parseExpression();
  skipCurrent(Kind::RightBracket); // current가 ] 토큰인지 확인
  return result;                   // 요소 접근 노드 반환
}


auto skipCurrent() -> void
{
  current++;
}

auto skipCurrent(Kind kind) -> void
{
  if (current->kind != kind)
  {
    cout << toString(kind) + " 토큰이 필요합니다.";
    exit(1);
  }
  current++;
}

auto skipCurrentIf(Kind token_kind) -> bool
{
  if (current->kind != token_kind)
    return false;
  current++;
  return true;
}


// x = 10;
// var x = 1;
// var x = a && b || c;
auto parseAssignment() -> Expression * // 대입 연산자 노드를 파싱하는 함수
{
  auto result = parseOr();               // 우선순위가 높은 OR 연산자 노드를 파싱
  if (current->kind != Kind::Assignment) // current가 = 토큰이 아니면
    return result;                       // OR 연산자 노드 반환
  skipCurrent(Kind::Assignment);         // current가 = 토큰이면
  if (auto getVariable = dynamic_cast<GetVariable *>(result))
  { // result가 GetVariable 노드라면 (result가 변수 이름이라면)
    /*
      dynamic_cast란?
      dynamic_cast는 C++에서 RTTI(Run-Time Type Information)를 지원하기 위해 사용하는 연산자이다.
    */
    auto result = new SetVariable();   // 대입 연산자 노드 생성
    result->name = getVariable->name;  // 대입 연산자 노드의 이름 설정
    result->value = parseAssignment(); // x = y = 10;과 같은 연쇄 대입을 위해
    return result;                     // 대입 연산자 노드 반환
  }
  if (auto getElement = dynamic_cast<GetElement *>(result))
  {
    auto result = new SetElement();
    result->sub = getElement->sub;
    result->index = getElement->index;
    result->value = parseAssignment();
    return result;
  }
  cout << "잘못된 대입식입니다.";
  exit(1);
}

auto parseExpression() -> Expression* {
  return parseAssignment();
};

auto parseExpressionStatement() -> ExpressionStatement* {
  auto result = new ExpressionStatement();
  result -> expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}


auto parseBlock() -> vector<Statement *> 
{
  vector<Statement *> result;               
  while (current->kind != Kind::RightBrace) 
  {
    switch (current->kind) 
    {
    default:
      result.push_back(parseExpressionStatement()); 
      break;
    case Kind::Variable:
      result.push_back(parseVariable()); 
      break;
    case Kind::For:
      result.push_back(parseFor()); 
      break;
    case Kind::If:
      result.push_back(parseIf()); 
      break;
    case Kind::Print:
    case Kind::PrintLine:
      result.push_back(parsePrint()); 
      break;
    case Kind::Return:
      result.push_back(parseReturn()); 
      break;
    case Kind::Break:
      result.push_back(parseBreak()); 
      break;
    case Kind::Continue:
      result.push_back(parseContinue()); 
      break;
    case Kind::EndOfToken:                     
      cout << *current << " 잘못된 구문입니다."; // 잘못된 구문이라고 출력
      exit(1);
    }
  }
  return result;
}

// for i = 0, i < 10, i = i + 1 {
auto parseFor() -> For *
{
  auto result = new For();                          // for문 노드 생성
  skipCurrent(Kind::For);                           // current가 for 토큰인지 확인
  result->variable = new Variable();                // 변수 선언문 노드 생성
  result->variable->name = current->string;         // 변수 이름 설정
  skipCurrent(Kind::Identifier);                    // current가 identifier(i) 토큰인지 확인
  skipCurrent(Kind::Assignment);                    // current가 = 토큰인지 확인
  result->variable->expression = parseExpression(); // 변수의 표현식을 파싱
  if (result->variable->expression == nullptr)      // 표현식이 없으면 for(var i;)와 같은 구문이므로 에러 메시지 출력
  {
    cout << "for문에 초기화식이 없습니다.";
    exit(1);
  }
  skipCurrent(Kind::Comma);              // current가 , 토큰인지 확인
  result->condition = parseExpression(); // 조건식을 파싱
  if (result->condition == nullptr)      // 조건식이 없으면 for(var i = 0;)와 같은 구문이므로 에러 메시지 출력
  {
    cout << "for문에 조건식이 없습니다.";
    exit(1);
  }
  skipCurrent(Kind::Comma);               // current가 , 토큰인지 확인
  result->expression = parseExpression(); // 증감식을 파싱
  if (result->expression == nullptr)      // 증감식이 없으면 for(var i = 0, i < 10;)와 같은 구문이므로 에러 메시지 출력
  {
    cout << "for문에 증감식이 없습니다.";
    exit(1);
  }
  skipCurrent(Kind::LeftBrace);  // current가 { 토큰인지 확인
  result->block = parseBlock();  // for문의 본문을 파싱
  skipCurrent(Kind::RightBrace); // current가 } 토큰인지 확인
  return result;
}

// 함수 노드를 파싱하는 함수
auto parseFunction()->Function* {
  auto result = new Function();
  skipCurrent(Kind::Function);
  result->name = current->string;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::LeftParen);
  if (current->kind != Kind::RightParen) {
    do {
      result->parameters.push_back(current->string);
      skipCurrent(Kind::Identifier);
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  result->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}


auto parse(vector<Token> tokens)->Program* {
  auto result = new Program();
  current = tokens.begin();
  while (current->kind != Kind::EndOfToken) {
    switch (current->kind) {
      case Kind::Function: {
        result->functions.push_back(parseFunction());
        break;
      }
      default: {
        cout << *current << " 잘못된 구문입니다.";
        exit(1);
      }
    }
  }
  return result;
}


// if문을 파싱하는 함수
// if x < 10 {
//   print(x);
// } else if x < 20 {
//   print(x);
// } else {
//   print(x);
// }
auto parseIf() -> If * // if문 노드를 파싱하는 함수
{
  auto result = new If(); // if문 노드 생성
  skipCurrent(Kind::If);  // current가 if 토큰인지 확인
  do                      // if문이 끝날 때까지 반복
  {
    auto condition = parseExpression(); // 조건식을 파싱
    if (condition == nullptr)           // 조건식이 없으면 if문에 조건식이 없다는 에러 메시지 출력
    {
      cout << "if문에 조건식이 없습니다.";
      exit(1);
    }
    result->conditions.push_back(condition); // 조건식을 if문 노드의 조건식 벡터에 추가
    skipCurrent(Kind::LeftBrace);            // current가 { 토큰인지 확인
    result->blocks.push_back(parseBlock());  // if문의 본문을 파싱하여 if문 노드의 본문 벡터에 추가
    skipCurrent(Kind::RightBrace);           // current가 } 토큰인지 확인
  } while (skipCurrentIf(Kind::Elif));       // current가 elif 토큰이면 다음 if문으로 넘어감. 아니면 (else 토큰이면) 반복문 종료
  if (skipCurrentIf(Kind::Else))             // current가 else 토큰이면
  {
    skipCurrent(Kind::LeftBrace);     // current가 { 토큰인지 확인
    result->elseBlock = parseBlock(); // else문의 본문을 파싱하여 if문 노드의 elseBlock에 설정
    skipCurrent(Kind::RightBrace);    // current가 } 토큰인지 확인
  }
  return result;
}

// print x, y, z;
auto parsePrint() -> Print * // print문 노드를 파싱하는 함수
{
  auto result = new Print();                           // print문 노드 생성
  result->lineFeed = current->kind == Kind::PrintLine; // current가 printLine 토큰이면 줄바꿈 여부를 true로 설정
  skipCurrent();                                       // current가 print 토큰이면 다음 토큰으로 넘어감
  if (current->kind != Kind::Semicolon)                // current가 ; 토큰이 아니면
  {
    do
      result->arguments.push_back(parseExpression()); // current가 ; 토큰이 나올 때까지 식 노드를 파싱하여 print문 노드의 인수로 설정
    while (skipCurrentIf(Kind::Comma));               // current가 , 토큰이면 다음 식 노드로 넘어감. 아니면 반복문 종료
  }
  skipCurrent(Kind::Semicolon); // current가 ; 토큰인지 확인
  return result;                // print문 노드 반환
}