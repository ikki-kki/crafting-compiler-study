#include "Interpreter.h"
/**
 * 컴파일 과정은 크게 전단부와 후단부로 나윔
 * 전단부에서는 소스 코드를 입력받아 어휘 분석(Lexical Analysis)을 통해 토큰 리스트를 생성하고, 
 * 이어서 구문 분석(Syntax Analysis)을 통해 구문 트리(Syntax Tree)를 만든다
 * 이 구문 트리는 컴파일 과정의 전단부와 후단부 사이를 잇는 매개체로 기능한다.
 * 
 * 인터프리터는 이 구문 트리를 실행하는 프로그램 
 * 즉, 인터프리터는 구문 트리를 순회하면서 각 노드를 실행하여 소스 코드를 실행한다,
 * 이 과정은 어휘 분석과 구문 분석을 거쳐 생성된 구문 트리를 바탕으로 이루어진다.
*/

// 유랭의 검색 순서
// 지역변수 > 전역변수 > 사용자 정의 함수 > 내장 함수


// --- 지역변수

// 선언된 위치에 따라 유효 스코프가 달라짐
// 지역변수는 선언된 블록 내에서만 유효
// 바깥쪽 list: 
//      - 각 요소가 함수의 블록을 나타낸다
//      - 함수의 호출 스택을 나타내고, 각 함수 호출에 대한 지역 변수의 집합
// 안쪽 list: 
//      - 각 요소가 특정 함수 블록 내부의 문(statement) 블록을 나타낸다
//      - 문 블록은 중괄호로 둘러싸인 코드 블록을 의미
//      - 중첩된 블록 구조엣서 변수의 유효 스코프를 관리할 수 있다.
// map<string, any>: 
//      - 각 맵은 변수의 이름(string)과 그 값(any)을 저장한다
static list<list<map<string, any>>> local;

// --- 전역변수
// pg87) 유랭은 선언 영역에 변수 선언을 허용하지 않으므로, 함수 내부에 선언되지 않은 변수는 자동으로 전역변수로 간주됩니다.

// 전통적 언어: 전역 변수를 프로그램 최상위에 명시적으로 선언함
// 유랭: 유랭과 같은 동적 타입 언어에서는 암시적 선언 방식을 채택함
static map<string, any> global;


// --- 함수의 이름과 함수 노드를 키와 값으로 가지는 전역 변수
static map<string, Function*> functionTable;


struct ReturnException { any result; };
struct BreakException {};
struct ContinueException {};


auto interpret(Program* program)-> void {
  // 프로그램 노드에 포함된 모든 함수를 전역 functionTable에 등록
  for (auto& node: program->functions)
    functionTable[node->name] = node;
  // 'main' 함수가 없으면 함수 종료
  if(functionTable["main"] == nullptr)
    return;
  
  try {
    // 새로운 지역 변수 스코프를 추가
    local.emplace_back().emplace_front();
    /**
     * 1도 몰라서 chatGPT한테 물어봤다..
     * local.emplace_back():
     * emplace_back 함수는 std::list의 멤버 함수입니다.
     * 이 함수는 local 리스트의 끝에 새로운 요소를 추가합니다.
     * 추가되는 요소의 타입은 local이 담고 있는 타입, 즉 list<map<string, any>>입니다.
     * emplace_back은 직접 새로운 요소를 생성하고 초기화하기 때문에, 
     * 새로운 list<map<string, any>> 객체가 local의 끝에 추가됩니다.
     * emplace_back()의 결과는 list<map<string, any>> 타입의 참조입니다. 
     * 이 참조에 대해 emplace_front()가 호출됩니다:
     * 
     * emplace_front 함수는 std::list의 멤버 함수로, 리스트의 시작 부분에 새로운 요소를 추가합니다.
     * 이 경우에는 새로 추가된 list<map<string, any>> 객체에 대해 호출되며, 
     * 이 리스트에 map<string, any> 타입의 새로운 요소를 맨 앞에 추가합니다.
    */
    // 'main' 함수 실행
    functionTable["main"]->interpret();
  } catch(ReturnException e){
    // 함수 실행 후 지역 변수 스코프 제거
    local.pop_back();
  };
}

// --- 함수 노드
// 단순 본문 노드 순회하며 interpret() 함수 호출
auto Function::interpret()->void {
  for (auto& node: block)
    node->interpret();
}

// --- 변수 노드
// 변수의 이름과 초기화식의 결과값을 키와 값으로 local 전역 변수에 등록
auto Variable::interpret()->void {
 local.back().front()[name] = expression->interpret();
 /**
  * .back():
  * 리스트의 마지막 요소에 대한 참조를 반환함
  * 여기서는 가장 최근에 호출된(실행중인)함수의 지역 변수 스코프를 나타냄
  * .front():
  * 현재 함수의 지역 변수 스코프 리스트의 첫 번쩌 요소에 접근함
  * 가장 내부(현재 실행 중인)코드 블록의 지역 변수 맵을 나타냄
  * [name]:
  * 변수의 이름을 키로 사용해 해당 변수의 값을 저장함
  * 맵에서 name 키에 해당하는 값을 찾고, 존재하지 않으면 새로운 키-쌍을 맴에 추가함.
 */
}

// --- 변수 참조 노드 //!NOTE: 지역 변수 전역 변수 선언 참조 테스트 시연
auto GetVariable::interpret()->any {
  // local의 마지막 요소에 있는 모든 변수 맵을 순회
  for (auto& variables: local.back()) {
    // 맵 내부에서 name이라는 키가 존재하는 지 확인
    if (variables.count(name))
      // 존재한다면 해당 변수의 값을 반환
      return variables[name];
    }
  // 지연 변수에서 찾지 못했다면 전역 변수에서 찾음
  if (global.count(name))
    // 존재한다면 해당 변수의 값을 반환
    return global[name];
  // 같은 이름으로 등록된 함수 노드가 있다면 해당 함수 노드를 반환
  if (functionTable.count(name))
    return functionTable[name];
  // if (builtinFunctionTable.count(name))
  // return builtinFunctionTable[name];
  // 지역, 전역 변수에 없다면 null을 반환한다
  return nullptr;
};

// --- 변수 수정 노드 //!NOTE: 지역 변수 전역 변수 선언 참조 테스트 시연
auto SetVariable::interpret()->any {
  // local의 마지막 요소에 있는 모든 변수 맵을 순회
  for (auto& variables: local.back()) {
    // 맵 내부에서 name이라는 키가 존재하는 지 확인
    if (variables.count(name))
      // 존재한다면 해당 변수에 새로운 값을 할당
      // value->interpret()는 value 객체의 interpret() 함수를 호출해 값을 구함
      return variables[name] = value->interpret();
  }
  // 지역 변수에 없다면 전역 변수에서 찾아 값을 할당
  return global[name] = value->interpret();
}

// --- 원소값 참조 노드 //!NOTE: 원소값 참조 테스트 시연
auto GetElement::interpret()->any {
  // 객체(배열 또는 맵) 인덱스 값을 평가
  auto object = sub->interpret();
  auto index_ = index->interpret();

  // 객체가 배열이고 인덱스가 숫자일 경우
  if (isArray(object) && isNumber(index_))
    // 배열의 인덱스에 해당하는 값을 반환
    return getValueOfArray(object, index_);
  // 객체가 맵이고 인덱스가 문자열일 경우
  if (isMap(object) && isString(index_))
    // 맵에서 해당 인덱스에 해당하는 값을 반환
    return getValueOfMap(object, index_);
  return nullptr;
}

// --- 원소값 수정 노드 //!NOTE: 원소값 수정 테스트 시연
auto SetElement::interpret()->any {
  // 원소 참조 노드와 마찬가지로 원소값의 수정을 표현하는 노드이므로
  // 피연산자 식 노드와 인덱스 식 노드를 가진다
  auto object = sub->interpret();
  auto index_ = index->interpret();
  // 값을 변경해야하므로 대입 식 노드도 추가로 가진다
  auto value_ = value->interpret();

  if (isArray(object) && isNumber(index_))
    // 해당 객체 내의 특정 위치에 값을 설정
    return setValueOfArray(object, index_, value_);
  if (isMap(object) && isString(index_))
    return setValueOfMap(object, index_, value_);
  return nullptr;
}


// --- for 문 노드 //!NOTE: For문 테스트 시연
// 블럭과 제어 변수를 가짐
auto For::interpret()->void{
  // 현재 실행중인 함수의 지역 변수 스코프인 local에 새로운 스코프를 추가
  local.back().emplace_front();
  // For 루프 제어 변수의 초기값을 구함
  variable->interpret();
  while (true) {
    auto result = condition->interpret();
    // 조건식이 거짓이면 루프를 종료
    if(isTrue(result) == false)
      break;
    try {
      // 블럭을 순회하며 interpret() 함수 호출
      for (auto& node: block)
        node->interpret();
    } catch (ContinueException) {
      /* empty */
    } catch (BreakException) {
      break;
    }

    expression->interpret();
  }
  // 실행이 끝나면 생성했던 문 블럭을 제거
  local.back().pop_front();
}

// --- if 문 노드 //!NOTE: if문 테스트 시연
auto If::interpret()->void {
  // 모든 조건을 순회
  for (auto i = 0; i < conditions.size(); i++) {
    // 조건을 평가
    auto result = conditions[i]->interpret();
    // 조건이 거짓이면 계속 진행 (다음 조건 확인)
    if (isTrue(result) == false)
      continue;

    // 새로운 지역 스코프 생성 (해당 if 블록의 스코프)
    local.back().emplace_front();
    // if 블록의 내용 실행
    for (auto& node: blocks[i])
      node->interpret();
    // 지역 스코프 제거
    local.back().pop_front();
    // if 블록 실행 후 함수 종료
    return;
  }

  // 모든 if 조건이 거짓이고 else 블록이 존재하면
  if (elseBlock.empty())
    return;
  
  // else 블록의 지역 스코프 생성
  local.back().emplace_front();
  // else 블록의 내용 실행
  for (auto& node: elseBlock)
    node->interpret();
  // else 블록의 지역 스코프 제거
  local.back().pop_front();
}

// --- Continue 문
auto Continue::interpret()->void {
  throw ContinueException();
}

// --- 함수 호출 문 노드 //!NOTE: 함수 호출 문 테스트 시연
auto Call::interpret()->any {
  // 함수(또는 내장 함수) 호출
  auto value = sub->interpret();
  // 내장 함수인 경우
  if (isBuiltinFunction(value)) {
    vector<any> values;
    // 모든 인자를 순회하며 interpret() 함수 호출
    for (size_t i = 0; i < arguments.size(); i++)
      values.push_back(arguments[i]->interpret());
    // 내장 함수 실행 및 결과 반환
    return toBuiltinFunction(value)(values);
  }

  // 일반 함수가 아닌 경우 
  if (isFunction(value) == false)
    // null 반환
    return nullptr;
  
  // 함수의 파라미터 매핑
  // 인자와 함께 함수를 호출할 수 있도록 매개 변수 이름과 인자값을 매핑시켜 보관한다.
  map<string, any> parameters;
  for (size_t i = 0; i < arguments.size(); i++) {
    auto name = toFunction(value)->parameters[i];
    parameters[name] = arguments[i]->interpret();
  }

  // 새로운 지역 스코프에 파라미터 추가
  local.emplace_back().push_front(parameters);
  try {
    // 함수 실행
    toFunction(value)->interpret();
  } catch (ReturnException exception) {
    // return문 처리: 지역 스코프 제거 및 결과 반환
    local.pop_back();
    return exception.result;
  }

  // 함수 실행 후 지역 스코프 제거
  local.pop_back();
  return nullptr;
}

// --- return 문 노드
auto Return::interpret()->void {
  // 반환 식 노드를 순회한 결과값을 예외 객체로 생성
  // 사용하려면 try-catch 문으로 감싸야 함
  throw ReturnException{expression->interpret()};
}


// --- print 문 노드
// 인터프리터는 파서와 달리 소스코드에 작성된 코드의 순서대로 노드를 방문함
// 따라서 모든 interpret() 함수 내용을 작성하지 않아도 실행 가능
// print문 노드가 가진 식 노드를 순화하며 interpret()
auto Print::interpret()->void {
  for (auto& node: arguments) {
    auto value = node->interpret();
    cout << value; // Use std::cout instead of cout
  }
  // 콘솔 개행 추가
  if (lineFeed) cout << endl; // Use std::cout instead of cout
}

auto StringLiteral::interpret()->any {
  return value;
}

// --- 배열 리터럴 노드
// 출력하기 위해선 operator<<() 함수를 오버로드해야 함
auto ArrayLiteral::interpret()->any {
  // 새로운 배열 객체 생성
  auto result = new Array();

  // 배열 리터럴의 모든 값들을 평가하고 배열에 추가
  for (auto& node: values)
    result->values.push_back(node->interpret());

  // 평가된 배열 반환
  return result;
}


// --- 산술 연산 노드   //!NOTE: 산술 연산 테스트 시연

// 양쪽 식 노드를 순회해 두 피연산자의 값을 구함
auto Arithmetic::interpret()->any {
  // 식 노드를 순회하며 interpret() 함수 호출
  auto lValue = lhs->interpret();
  auto rValue = rhs->interpret();
  // 연산자가 덧셈이고 피연산자가 모두 숫자일 경우
   if (kind == Kind::Add && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) + toNumber(rValue);
  }
  // 연산자가 덧셈(+)이고  피연산자가 모두 문자열일 경우
  if (kind == Kind::Add && isString(lValue) && isString(rValue)) {
    return toString(lValue) + toString(rValue);
  }
  //연산자가 뺄셈(-)이고 피연산자가 모두 숫자일 경우
  if (kind == Kind::Subtract && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) - toNumber(rValue);
  }
  // 연산자가 곱셈(*)이고 피연산자가 모두 숫자일 경우
  if (kind == Kind::Multiply && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) * toNumber(rValue);
  }
  // 연산자가 나눗셈(%)이고 피연산자가 모두 숫자일 경우
    if (kind == Kind::Divide && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(rValue) == 0 ? 0.0 : toNumber(lValue) / toNumber(rValue);
  }
  // 연산자가 나머지(%)이고 피연산자가 모두 숫자일 경우
    if (kind == Kind::Modulo && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(rValue) == 0 ? toNumber(lValue) : fmod(toNumber(lValue), toNumber(rValue));
  }
  return 0.0;

}

// --- 논리 연산 노드 //!NOTE: 논리 연산 테스트 시연

// --- OR 연산자
// 왼쪽 식이 참이면 오른쪽 평가 X
auto Or::interpret()->any {
  return isTrue(lhs->interpret()) ? true : rhs->interpret();
}

// --- AND 연산자 노드
// 왼쪽 식이 거짓이면 거짓 반환
// 둘 다 참이어야 참 반환
auto And::interpret()->any {
  return isFalse(lhs->interpret()) ? false : rhs->interpret();
}

// -------여기서부턴 책에 없는 내용들---------

auto MapLiteral::interpret()->any {
  auto result = new Map();
  for (auto& [key, value]: values)
    result->values.insert_or_assign(key, value->interpret());
  return result;
}

auto NullLiteral::interpret()->any {
  return nullptr;
}

auto BooleanLiteral::interpret()->any {
  return value;
}

auto NumberLiteral::interpret()->any {
  return value;
}

auto ExpressionStatement::interpret()->void {
  expression->interpret();
}

auto Break::interpret()->void {
  throw BreakException();
}

auto Unary::interpret()->any {
  auto value = sub->interpret();
  if (kind == Kind::Add && isNumber(value))
    return abs(toNumber(value));
  if (kind == Kind::Subtract && isNumber(value))
    return toNumber(value) * -1;
  return 0.0;
}

auto Relational::interpret()->any {
  auto lValue = lhs->interpret();
  auto rValue = rhs->interpret();
  if (kind == Kind::Equal && isNull(lValue) && isNull(rValue)) {
    return true;
  }
  if (kind == Kind::Equal && isBoolean(lValue) && isBoolean(rValue)) {
    return toBoolean(lValue) == toBoolean(rValue);
  }
  if (kind == Kind::Equal && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) == toNumber(rValue);
  }
  if (kind == Kind::Equal && isString(lValue) && isString(rValue)) {
    return toString(lValue) == toString(rValue);
  }
  if (kind == Kind::NotEqual && isNull(lValue) && isNull(rValue)) {
    return false;
  }
  if (kind == Kind::NotEqual && isNull(lValue) || isNull(rValue)) {
    return true;
  }
  if (kind == Kind::NotEqual && isBoolean(lValue) && isBoolean(rValue)) {
    return toBoolean(lValue) != toBoolean(rValue);
  }
  if (kind == Kind::NotEqual && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) != toNumber(rValue);
  }
  if (kind == Kind::NotEqual && isString(lValue) && isString(rValue)) {
    return toString(lValue) != toString(rValue);
  }
  if (kind == Kind::LessThan && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) < toNumber(rValue);
  }
  if (kind == Kind::GreaterThan && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) > toNumber(rValue);
  }
  if (kind == Kind::LessOrEqual && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) <= toNumber(rValue);
  }
  if (kind == Kind::GreaterOrEqual && isNumber(lValue) && isNumber(rValue)) {
    return toNumber(lValue) >= toNumber(rValue);
  }
  return false;
}

/**
 * 마치며
 * 
 * 이번 장에서는 구문 트리를 순회하며 노드들을 실행하는 인터프리터를 만들었다.
 * 
*/