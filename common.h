#include <iostream>
#include <vector>
#include <unordered_map>
#include <variant>
#include <stdint.h>
#include <memory>
#include <variant>
#include <string>


using std::string;
using std::unordered_map;
using std::vector;
using std::cout;
using std::endl;
using std::get;
using std::holds_alternative;
using std::stringstream;
using std::variant;

using i64 = long long;
using Value = variant<std::monostate, bool, double, i64,
      string, char>;

