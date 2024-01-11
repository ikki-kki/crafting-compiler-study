#include "Datatype.h"

/**
 * 유랭은 동적 타입 언어지만 정적 타입 표현을 위해 any를 사용한다
 * 보조함수
 * isXXX() : any 타입에 들어있는내부 값을 확인한다
 * toXXX() : any 타입에 들어있는 내부 값을 캐스팅한다
 * 
 * 이는 타입 안정성을 보장하면서 any 내부의 값을 사용하려 할 떄 유용
*/


auto isSize(any value)->bool {
  return value.type() == typeid(size_t);
}

auto toSize(any value)->size_t {
  return any_cast<size_t>(value);
}

auto isNull(any value)->bool {
  return value.type() == typeid(nullptr_t);
}

auto isTrue(any value)->bool {
  return isBoolean(value) && toBoolean(value);
}

auto isFalse(any value)->bool {
  return isBoolean(value) && toBoolean(value) == false;
}

auto isBoolean(any value)->bool {
  return value.type() == typeid(bool);
}

auto toBoolean(any value)->bool {
  return any_cast<bool>(value);
}

auto isNumber(any value)->bool {
  return value.type() == typeid(double);
}

auto toNumber(any value)->double {
  return any_cast<double>(value);
}

auto isString(any value)->bool {
  return value.type() == typeid(string);
}

auto toString(any value)->string {
  return any_cast<string>(value);
}

auto isArray(any value)->bool {
  return value.type() == typeid(Array*);
}

auto toArray(any value)->Array* {
  return any_cast<Array*>(value);
}

auto getValueOfArray(any object, any index)->any {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object)->values.size())
    return toArray(object)->values[i];
  return nullptr;
}

auto setValueOfArray(any object, any index, any value)->any {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object)->values.size())
    toArray(object)->values[i] = value;
  return value;
}

auto isMap(any value)->bool {
  return value.type() == typeid(Map*);
}

auto toMap(any value)->Map* {
  return any_cast<Map*>(value);
}

auto getValueOfMap(any object, any key)->any {
  if (toMap(object)->values.count(toString(key)))
    return toMap(object)->values[toString(key)];
  return nullptr;
}

auto setValueOfMap(any object, any key, any value)->any {
  toMap(object)->values[toString(key)] = value;
  return value;
}

auto isFunction(any value)->bool {
  return value.type() == typeid(Function*);
}

auto toFunction(any value)->Function* {
  return any_cast<Function*>(value);
}

auto isBuiltinFunction(any value)->bool {
  return value.type() == typeid(function<any(vector<any>)>);
}

auto toBuiltinFunction(any value)->function<any(vector<any>)> {
  return any_cast<function<any(vector<any>)>>(value);
}

//  C++의 스트림 삽입 연산자를 오버로드하여 std::any 타입의 값들을 
//적절한 형식으로 출력 스트림에 삽입한다. 
auto operator<<(ostream& stream, any& value)->ostream& {
  if (isNull(value)) {
    stream << "null";
  }
  else if (isBoolean(value)) {
    stream << boolalpha << any_cast<bool>(value);
  }
  else if (isNumber(value)) {
    printf("%g", toNumber(value));
  }
  else if (isString(value)) {
    stream << toString(value);
  }
  else if (isArray(value)) {
    stream << "[ ";
    for (auto& value : toArray(value)->values)
      stream << value << " ";
    stream << "]";
  }
  else if (isMap(value)) {
    stream << "{ ";
    for (auto& [key, value] : toMap(value)->values)
      stream << key << ":" << value << " ";
    stream << "}";
  }
  return stream;
}