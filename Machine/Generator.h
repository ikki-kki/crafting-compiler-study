#include <map>
#include <list>
#include <tuple>
#include <vector>
#include <iomanip>
#include "Node.h"
#include "Code.h"

using std::max;
using std::cout;
using std::setw;
using std::endl;
using std::map;
using std::list;
using std::tuple;
using std::vector;

static auto getLocal(string)->size_t;
static auto setLocal(string)->void;
static auto initBlock()->void;
static auto pushBlock()->void;
static auto popBlock()->void;
static auto writeCode(Instruction)->size_t;
static auto writeCode(Instruction, any)->size_t;
static auto patchAddress(size_t)->void;
static auto patchOperand(size_t, size_t)->void;

static vector<Code> codeList;
static map<string, size_t> functionTable;
static list<map<string, size_t>> symbolStack;
static vector<size_t> offsetStack;
static size_t localSize;
static vector<vector<size_t>> continueStack;
static vector<vector<size_t>> breakStack;