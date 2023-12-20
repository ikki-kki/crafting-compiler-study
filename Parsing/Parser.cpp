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

// 토큰이 현재 토큰과 일치하는지 확인하는 함수
auto skipCurrent(Kind kind) -> void {
  if(current -> kind != kind){
    cout << toString(kind) + " 토큰이 필요합니다.";
    exit(1);
  }
  current++;
}

auto parseVariable() -> Variable* {
  auto result = new Variable();
  skipCurrent(Kind::Variable);
  result -> name = current -> string;

};

auto parseExpression() -> Expression *{
  return parseAssignment();
}


// 논리 or 연산자
auto parseOr() -> Expression* {
  auto result = parseAnd();
  while (skipCurrentIf(Kind::LogicalOr)){
    auto temp = new Or();
    temp-> lhs = result;
    temp-> rhs = parseAnd();
    result = temp;
  }
  return result;
}


// 대입 연산자
// x = 10;
// var x = 1;
// var x = a && b || c;
auto parseAssignment()->Expression* {
  auto result = parseOr();
  if (current->kind != Kind::Assignment)
    return result;
  skipCurrent(Kind::Assignment);
  if (auto getVariable = dynamic_cast<GetVariable*>(result)) {
    auto result = new SetVariable();
    result->name = getVariable->name;
    result->value = parseAssignment();
    return result;
  }
  if (auto getElement = dynamic_cast<GetElement*>(result)) {
    auto result = new SetElement();
    result->sub = getElement->sub;
    result->index = getElement->index;
    result->value = parseAssignment();
    return result;
  }
  cout << "잘못된 대입 연산 식입니다.";
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