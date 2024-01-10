#include <any>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include "Datatype.h"
#include <cmath>

using std::any;
using std::map;
using std::string;
using std::vector;
using std::function;



/**
 * builtinFunctionTable은 프로그래밍 언어네 기본적으로 내장된 함수들을 저장하는 테이블이다.
 * 내장 함수들은 추가적인 선언 없이 직접 사용될 수 있다.
 * 함수 호출 처리를 할 때 변수 이름을 찾는 과정에서 이 테이블을 확인하고, 만약 요청된 이름이 해당 테이블에 존재한다면
 * 이름과 연결된 함수 객체가 반환되어 이를 통해 유랭은 내장 함수를 호출할 수 있다.
*/
map<string, function<any(vector<any>)>> builtinFunctionTable = {
  {"length", [](vector<any> values)->any {
    if (values.size() == 1 && isArray(values[0]))
      return static_cast<double>(toArray(values[0])->values.size());
    if (values.size() == 1 && isMap(values[0]))
      return static_cast<double>(toMap(values[0])->values.size());
    return 0.0;
  }},
  {"push", [](vector<any> values)->any {
    if (values.size() == 2 && isArray(values[0])) {
      toArray(values[0])->values.push_back(values[1]);
      return values[0];
    }
    return nullptr;
  }},
  {"pop", [](vector<any> values)->any {
    if (values.size() == 1 && isArray(values[0]) && toArray(values[0])->values.size() != 0) {
      auto result = toArray(values[0])->values.back();
      toArray(values[0])->values.pop_back();
      return result;
    }
    return nullptr;
  }},
  {"erase", [](vector<any> values)->any {
    if (values.size() == 2 && isMap(values[0]) && isString(values[1]) &&
        toMap(values[0])->values.count(toString(values[1]))) {
      auto result = toMap(values[0])->values.at(toString(values[1]));
      toMap(values[0])->values.erase(toString(values[1]));
      return result;
    }
    return nullptr;
  }},
  {"clock", [](vector<any> values)->any {
    return static_cast<double>(clock());
  }},
  {"sqrt", [](vector<any> values)->any {
    return sqrt(toNumber(values[0]));
  }},
};