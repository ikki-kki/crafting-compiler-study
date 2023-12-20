#include <map>
#include <iomanip>
#include "Kind.h"

using namespace std;


static map<string, Kind> stringToKind = {
    {"#unkown", Kind::Unknown},
    {"#EndOfToken", Kind::EndOfToken},

    {"null", Kind::NullLiteral},
    {"true", Kind::TrueLiteral},
    {"false", Kind::FalseLiteral},
    {"#Number", Kind::NumberLiteral},
    {"#String", Kind::StringLiteral},
    {"#identifier", Kind::Identifier},

    {"function", Kind::Function},
    {"return", Kind::Return},
    {"var", Kind::Variable},
    {"for", Kind::For},
    {"break", Kind::Break},
    {"continue", Kind::Continue},
    {"if", Kind::If},
    {"elif", Kind::Elif},
    {"else", Kind::Else},
    {"print", Kind::Print},
    {"printLine", Kind::PrintLine},

    {"and", Kind::LogicalAnd},
    {"or", Kind::LogicalOr},

    {"=", Kind::Assignment},

    {"+", Kind::Add},
    {"-", Kind::Subtract},
    {"*", Kind::Multiply},
    {"/", Kind::Divide},
    {"%", Kind::Modulo},

    {"==", Kind::Equal},
    {"!=", Kind::NotEqual},
    {"<", Kind::LessThan},
    {">", Kind::GreaterThan},
    {"<=", Kind::LessOrEqual},
    {">=", Kind::GreaterOrEqual},

    {",", Kind::Comma},
    {":", Kind::Colon},
    {";", Kind::Semicolon},
    {"(", Kind::LeftParen},
    {")", Kind::RightParen},
    {"{", Kind::LeftBrace},
    {"}", Kind::RightBrace},
    {"[", Kind::LeftBracket},
    {"]", Kind::RightBracket},
};

static auto kindToString = [] {
  map<Kind, string> result;
  for (auto& [key, value] : stringToKind)
    result[value] = key;
  return result;
}();

Kind toKind(string string) {
  if (stringToKind.count(string))
    return stringToKind.at(string);
  return Kind::Unknown;
}

string toString(Kind kind) {
  if (kindToString.count(kind))
    return kindToString.at(kind);
  return "";
}
