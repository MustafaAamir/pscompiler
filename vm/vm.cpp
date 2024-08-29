#include "vm.h"
#include "../common.h"
#include <variant>
#include <algorithm>
#include <random>

inline bool VirtualMachine::isNumber(Value v) {
    return (holds_alternative<double>(v) || holds_alternative<i64> (v));
}

template<typename T>
inline bool VirtualMachine::isType(Value v) {
    return (holds_alternative<T>(v));
}

inline Value VirtualMachine::pop() {
    const auto value = valueStack.back();
    valueStack.pop_back();
    return value;
}

string trim(const string& str, const string &whitespace) {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}


inline void VirtualMachine::LogicalBinOp(Value v1, Value v2, char op) {
    if (isType<bool>(v1) && isType<bool>(v2)) {
        const bool rightOperand = get<bool>(pop());
        const bool leftOperand = get<bool>(pop());
        switch (op) {
            case ('&'): valueStack.push_back(leftOperand && rightOperand); break;
            case ('|'): valueStack.push_back(leftOperand || rightOperand); break;
            default:    Error.report(position, "Runtime", "Unrecognized logical operand");
        }
    } else {
        Error.report(position, "Runtime", "Invalid arguments to logical operators.");
    }
}

inline void VirtualMachine::Builtin() {
    char name = get<char>(pop());
    if (name == builtintype::Mid) {
        i64 len, startpos;
        string str;
        if (isType<i64>(valueStack.back())) {
            len      = get<i64>(pop());
        } else {
            Error.report(position, "Runtime", "Expected 'startpos' in MID(<string>, <startpos>, <length>) to be of type INTEGER");
        }
        if (isType<i64>(valueStack.back())) {
            startpos = get<i64>(pop());
        } else {
            Error.report(position, "Runtime", "Expected 'length' in MID(<string>, <startpos>, <length>) to be of type INTEGER");
        }
        if (isType<string>(valueStack.back())) {
            str = get<string>(pop());
        } else {
            Error.report(position, "Runtime", "Expected 'string' in MID(<string>, <startpos>, <length>) to be of type INTEGER");
        }
        if (len > str.length() || len + startpos > str.length()) {
            Error.report(position, "Index out of bounds", "substring length exceeds string length");
        }
        valueStack.push_back(str.substr(startpos, len));
        return;
    } else if (name == builtintype::Reverse) {
        if (isType<string>(valueStack.back())) {
            string name = get<string>(pop());
            std::reverse(name.begin(), name.end());
            valueStack.push_back(name);
        } else {
            Error.report(position, "Runtime", "Invalid arguments to Reverse(<STRING>)");
        }
        return;
    } else if (name == builtintype::Length) {
        if (isType<string>(valueStack.back())) {
            string name = get<string>(pop());
            valueStack.push_back((i64)name.length());
        } else if (isType<char>(valueStack.back())) {
            valueStack.back() = (i64)1;
        } else {
            Error.report(position, "Runtime", "Invalid argument to 'LENGTH' function. LENGTH takes an orgument of type CHAR or STRING");
        }
        return;
    } else if (name == builtintype::Sin) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back((double)std::sin(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back((double)std::sin(get<double>(pop())));
        } else {
            stringstream ss;
            ss << "use of unary operator 'Sin' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
       }
        return;
    } else if (name == builtintype::Cos) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back((double)std::cos(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back((double)std::cos(get<double>(pop())));
       } else {
            stringstream ss;
            ss << "use of unary operator 'Cos' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
       }
       return;
    } else if (name == builtintype::Tan) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back((double)std::tan(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back((double)std::tan(get<double>(pop())));
       } else {
            stringstream ss;
            ss << "use of unary operator 'Tan' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
       }
    } else if (name == builtintype::Sqrt) {
        if (isType<i64>(valueStack.back())) {
            if (get<i64>(valueStack.back()) < 0) {
                stringstream ss;
                ss << "use of unary operator 'Sqrt' on negative value'"  << valueStack.back() << "' is not allowed";
                Error.report(position, "Runtime", ss.str());
            }
            valueStack.push_back((double)std::sqrt(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            if (get<double>(valueStack.back()) < 0.0) {
                stringstream ss;
                ss << "use of unary operator 'Sqrt' on negative value'"  << valueStack.back() << "' is not allowed";
                Error.report(position, "Runtime", ss.str());
            }
            valueStack.push_back(std::sqrt(get<double>(pop())));
       } else {
            stringstream ss;
            ss << "use of unary operator 'Sqrt' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
       }
       return;
    } else if (name == builtintype::Abs) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back(std::abs(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back(std::abs(get<double>(pop())));
       } else {
            stringstream ss;
            ss << "use of unary operator 'Abs' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
       }
       return;
    } else if (name == builtintype::IntegerCast) {
        if (isType<i64>(valueStack.back())) {
            return;
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back((i64)get<double>(pop()));
        } else {
            stringstream ss;
            ss << "use of Cast 'integer_cast' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
        }
        return;
    } else if (name == builtintype::RealCast) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back((double)get<i64>(pop()));
        } else if (isType<double>(valueStack.back())) {
            return;
        } else {
            stringstream ss;
            ss << "use of Cast 'REAL()' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
        }
        return;
     } else if (name == builtintype::StringCast) {
        if (isType<i64>(valueStack.back())) {
            valueStack.push_back(std::to_string(get<i64>(pop())));
        } else if (isType<double>(valueStack.back())) {
            valueStack.push_back(std::to_string(get<double>(pop())));
        } else if (isType<char>(valueStack.back())) {
            valueStack.push_back((string(1, get<char>(pop()))));
        } else if (isType<string>(valueStack.back())) {
            return;
        } else {
            stringstream ss;
            ss << "use of Cast 'STRING()' on '"  << valueStack.back() << "' is not allowed";
            Error.report(position, "Runtime", ss.str());
        }
        return;
     } else if (name == builtintype::RandomInt) {
         i64 ub = get<i64>(pop());
         i64 lb = get<i64>(pop());
         std::mt19937 twister(rand());
         std::uniform_int_distribution<int> dist(lb,ub);
         valueStack.push_back(dist(twister));
         return;
     } else if (name == builtintype::RandomReal) {
         i64 ub = get<i64>(pop());
         i64 lb = get<i64>(pop());
         std::uniform_real_distribution<double> unif(lb,ub);
         std::default_random_engine re;
         double val = unif(re);
         valueStack.push_back(val);
         return;
     }
    }

inline void VirtualMachine::BinOp(Value v1, Value v2, char op) {
    if (isType<i64>(v1) && isType<i64>(v2)) {
        const auto rightOperand = get<i64>(pop());
        const auto leftOperand = get<i64>(pop());
        switch (op) {
            case ('+'): valueStack.push_back(leftOperand + rightOperand); break;
            case ('-'): valueStack.push_back(leftOperand - rightOperand); break;
            case ('*'): valueStack.push_back(leftOperand * rightOperand); break;
            case ('/'): valueStack.push_back(leftOperand / rightOperand); break;
            case ('%'): valueStack.push_back(leftOperand % rightOperand); break;
            case ('d'): valueStack.push_back(leftOperand / rightOperand); break;
            default:    Error.report(position, "Runtime", "Unrecognized arithmetic operand");
        }
    } else if (isType<double>(v1) && isType<double>(v2)) {
        const auto rightOperand = get<double>(pop());
        const auto leftOperand = get<double>(pop());
        switch (op) {
            case ('+'): valueStack.push_back(leftOperand + rightOperand); break;
            case ('-'): valueStack.push_back(leftOperand - rightOperand); break;
            case ('*'): valueStack.push_back(leftOperand * rightOperand); break;
            case ('/'): valueStack.push_back(leftOperand / rightOperand); break;
            case ('%'): valueStack.push_back(fmod(leftOperand, rightOperand)); break;
            case ('d'): valueStack.push_back(leftOperand / rightOperand); break;
            default:    Error.report(position, "Runtime", "Unrecognized arithmetic operand");
        }
    } else if (isNumber(v1) && isNumber(v2)) {
        string s(1, op);
        Error.report(position, "Runtime", "binary operand '" + s + "' cannot be used between Real and Integer");
    } else {
        string s(1, op);
        Error.report(position, "Runtime", "binary operand '" + s + "' cannot be used between non-numerical types");
    }
}


inline void VirtualMachine::Concatenate(Value v1, Value v2) {
    if (isNumber(v1) || isNumber(v2)) {
        Error.report(position, "Runtime", "binary operand '&' cannot be used with Integer or Real");
    }
    else if (isType<bool>(v1) || isType<bool>(v2)) {
        Error.report(position, "Runtime", "binary operand '&' cannot be used with Boolean");
    }
    if (isType<string>(v1)) {
        auto rightOperand = get<string>(pop());
        if (isType<string>(v2)) {
            auto leftOperand = get<string>(pop());
            valueStack.push_back(leftOperand + rightOperand);
        } else {
            auto leftOperand = get<char>(pop());
            valueStack.push_back(leftOperand + rightOperand);
        }
    } else {
        auto rightOperand = get<char>(pop());
        if (isType<string>(v2)) {
            auto leftOperand = get<string>(pop());
            valueStack.push_back(leftOperand + rightOperand);
        } else {
            auto leftOperand = string(1, get<char>(pop()));
            leftOperand.push_back(rightOperand);
            valueStack.push_back(leftOperand);
        }
    }

    return;
}

void VirtualMachine::run() {
    for (offset = 0; offset < chunk->bytecode.size();) {
        position = compiler.chunkPosition[offset];
        const auto opCode = static_cast<OpCode>(chunk->read(offset++));
        printValueStack(opCode);
        switch (opCode) {
            case (OpCode::Builtin): {
                Builtin();
                break;
                                    }
            case (OpCode::Constant): {
                const auto idx = static_cast<uint16_t>(chunk->read(offset++));
                const auto idx1 = static_cast<uint16_t>(chunk->read(offset++));
                valueStack.push_back(chunk->getConstant(((idx << 8) & 0xff00) | (idx1 & 0xff)));
                break;
                                     }
            case (OpCode::Call): {
                const auto idx = static_cast<uint16_t>(chunk->read(offset++));
                const auto idx1 = static_cast<uint16_t>(chunk->read(offset++));
                const auto nidx = chunk->getConstant(((idx << 8) & 0xff00) | (idx1 & 0xff));

                reg = offset;
                offset = get<i64>(nidx);
                break;
                                 }
            case (OpCode::EndFunction): {
                offset = reg;
                break;
                                        }

            case (OpCode::DefineLocal): {
                const string name = get<string>(pop());
                auto it = locals.find(name);
                if (it != locals.end()) {
                    Error.report(position, "Runtime", "Local '" + name + "' already defined");
                }
                locals.emplace(name, std::monostate{});
                break;
                                        }
            case (OpCode::DefineLocalArray): {
                auto name = get<string>(pop());
                auto ub = get<i64>(pop());
                auto lb = get<i64>(pop());
                std::vector<Value> arr(ub - lb + 1);
                auto it = locals.find(name);
                if (it != locals.end()) {
                    Error.report(position, "Runtime", "Local '" + name + "' already defined");
                }
                locals.emplace(name, std::monostate{});
                valueArrayMap.emplace(name, std::make_unique<ValueArray>(arr, ub, lb, name));
                break;
                                             }

            case (OpCode::DefineGlobal): {
                const string name = get<string>(pop());
                auto it = globals.find(name);
                if (it != globals.end()) {
                    Error.report(position, "Runtime", "Global '" + name + "' already defined");
                }
                globals.emplace(name, std::monostate{});
                break;
                                         }
            case (OpCode::DefineGlobalArray): {
                auto name = get<string>(pop());
                auto ub = get<i64>(pop());
                auto lb = get<i64>(pop());
                std::vector<Value> arr(ub - lb + 1);
                auto it = globals.find(name);
                if (it != globals.end()) {
                    Error.report(position, "Runtime", "Global '" + name + "' already defined");
                }
                globals.emplace(name, std::monostate{});
                valueArrayMap.emplace(name, std::make_unique<ValueArray>(arr, ub, lb, name));
                break;
                                              }

            case (OpCode::SetGlobal): {
                const Value newValue = pop();
                const string name = get<string>(valueStack.back());
                const auto it = globals.find(name);
                if (it == globals.end()) {
                    Error.report(position, "Runtime", "global '" + name + "' is undefined");
                }
                if (holds_alternative<i64>(newValue) && compiler.globalsType[name] == TokenType::Integer) {
                   globals[name] = get<i64>(newValue);
                } else if (holds_alternative<bool>(newValue) && compiler.globalsType[name] == TokenType::Boolean) {
                   globals[name] = get<bool>(newValue);
                } else if (holds_alternative<string>(newValue) && compiler.globalsType[name] == TokenType::String) {
                   globals[name] = get<string>(newValue);
                } else if (holds_alternative<double>(newValue) && compiler.globalsType[name] == TokenType::Real) {
                   globals[name] = get<double>(newValue);
                } else if (holds_alternative<char>(newValue) && compiler.globalsType[name] == TokenType::Char) {
                   globals[name] = get<char>(newValue);
                } else {
                    stringstream ss;
                    ss << "type of global '" << name << "' is incompatible with " << newValue;
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                      }

            case (OpCode::SetGlobalArray): {
                const Value newValue = pop();
                const string name  = get<string>(pop());
                const i64 index    = get<i64>(pop());

                auto it = valueArrayMap.find(name);
                if (it == valueArrayMap.end()) {
                    Error.report(position, "Runtime", "global array'" + name + "' is undefined");
                }
                if (it->second->ub < index || it->second->lb > index ) {
                    Error.report(position, "Out of bounds", "index '" + std::to_string(index)
                            + "' is out of bounds for " + name + "[" + std::to_string(it->second->lb)
                            + ":" + std::to_string(it->second->ub) + "]");
                }
                if (holds_alternative<i64>(newValue) && compiler.globalsType[name] == TokenType::Integer) {
                    it->second->array[index] = get<i64>(newValue);
                } else if (holds_alternative<bool>(newValue) && compiler.globalsType[name] == TokenType::Boolean) {
                   it->second->array[index] = get<bool>(newValue);
                } else if (holds_alternative<string>(newValue) && compiler.globalsType[name] == TokenType::String) {
                   it->second->array[index] = get<string>(newValue);
                } else if (holds_alternative<double>(newValue) && compiler.globalsType[name] == TokenType::Real) {
                   it->second->array[index] = get<double>(newValue);
                } else if (holds_alternative<char>(newValue) && compiler.globalsType[name] == TokenType::Char) {
                   it->second->array[index] = get<char>(newValue);
                } else {
                    stringstream ss;
                    ss << "type of global '" << name << "' is incompatible with array " << newValue;
                    Error.report(position, "Runtime", ss.str());
                }
                valueStack.emplace_back(newValue);
                break;
                                           }

            case (OpCode::GetGlobal): {
                //cout << "Hahahaha in getglobal, offset = " << offset << endl;
                string name = get<string>(valueStack.back());
                const auto it = globals.find(name);
                if (it == globals.end()) {
                    Error.report(position, "Runtime", "global '" + name + "' is undefined");
                }
                if (isType<std::monostate>(it->second)) {
                    Error.report(position, "Runtime", "global identifier '" + name + "' is unbound");
                }
                valueStack.back() = it->second;
                break;
                                      }

            case (OpCode::GetGlobalArray): {
                string name = get<string>(pop());
                i64   index = get<i64>(pop());
                const auto it2 = globals.find(name);
                if (it2 == globals.end()) {
                    Error.report(position, "Runtime", "global array'" + name + "' is undefined");
                }
                const auto it = valueArrayMap.find(name);
                if (it == valueArrayMap.end()) {
                    Error.report(position, "Runtime", "global Array'" + name + "' is unbound");
                }
                if (index > it->second->ub || index < it->second->lb) {
                    Error.report(position, "Out of bounds", "index '" + std::to_string(index)
                            + "' is out of bounds for " + name + "[" + std::to_string(it->second->lb)
                            + ":" + std::to_string(it->second->ub) + "]");
                }
                valueStack.emplace_back(it->second->array[index]);
                break;
                                           }
            case (OpCode::GetLocal): {
                string name = get<string>(valueStack.back());
                auto it = locals.find(name);
                if (it == locals.end()) {
                    Error.report(position, "Runtime", "local '" + name + "' is undefined");
                }
                if (isType<std::monostate>(it->second)) {
                    Error.report(position, "Runtime", "local identifier '" + name + "' is unbound");
                }
                valueStack.back() = it->second;
                break;
                                     }
            case (OpCode::GetLocalArray): {
                string name = get<string>(pop());
                i64   index = get<i64>(pop());
                const auto it2 = locals.find(name);
                if (it2 == locals.end()) {
                    Error.report(position, "Runtime", "local array'" + name + "' is undefined");
                }
                const auto it = valueArrayMap.find(name);
                if (it == valueArrayMap.end()) {
                    Error.report(position, "Runtime", "local Array'" + name + "' is unbound");
                }
                if (index > it->second->ub || index < it->second->lb) {
                    Error.report(position, "Out of bounds", "index '" + std::to_string(index)
                            + "' is out of bounds for " + name + "[" + std::to_string(it->second->lb)
                            + ":" + std::to_string(it->second->ub) + "]");
                }
                valueStack.emplace_back(it->second->array[index]);
                break;

                                          }

            case (OpCode::SetLocal): {
                const Value newValue = pop();
                const string name = get<string>(valueStack.back());
                const auto it = locals.find(name);
                if (it == locals.end()) {
                    Error.report(position, "Runtime", "local identifier '" + name + "' is undefined");
                }
                if (holds_alternative<i64>(newValue) && compiler.localsType[name] == TokenType::Integer) {
                   locals[name] = get<i64>(newValue);
                } else if (holds_alternative<bool>(newValue) && compiler.localsType[name] == TokenType::Boolean) {
                   locals[name] = get<bool>(newValue);
                } else if (holds_alternative<string>(newValue) && compiler.localsType[name] == TokenType::String) {
                   locals[name] = get<string>(newValue);
                } else if (holds_alternative<double>(newValue) && compiler.localsType[name] == TokenType::Real) {
                   locals[name] = get<double>(newValue);
                } else if (holds_alternative<char>(newValue) && compiler.localsType[name] == TokenType::Char) {
                   locals[name] = get<char>(newValue);
                } else {
                    stringstream ss;
                    ss << "type of local '" << name << "' is incompatible with " << newValue;
                    Error.report(position, "Runtime", ss.str());
                }
                break;
            }

            case (OpCode::SetLocalArray): {
                const Value newValue = pop();
                const string name  = get<string>(pop());
                const i64 index    = get<i64>(pop());

                auto it = valueArrayMap.find(name);
                if (it == valueArrayMap.end()) {
                    Error.report(position, "Runtime", "local array'" + name + "' is undefined");
                }
                if (it->second->ub < index || it->second->lb > index ) {
                    Error.report(position, "Out of bounds", "index '" + std::to_string(index)
                            + "' is out of bounds for " + name + "[" + std::to_string(it->second->lb)
                            + ":" + std::to_string(it->second->ub) + "]");
                }
                if (holds_alternative<i64>(newValue) && compiler.localsType[name] == TokenType::Integer) {
                    it->second->array[index] = get<i64>(newValue);
                } else if (holds_alternative<bool>(newValue) && compiler.localsType[name] == TokenType::Boolean) {
                   it->second->array[index] = get<bool>(newValue);
                } else if (holds_alternative<string>(newValue) && compiler.localsType[name] == TokenType::String) {
                   it->second->array[index] = get<string>(newValue);
                } else if (holds_alternative<double>(newValue) && compiler.localsType[name] == TokenType::Real) {
                   it->second->array[index] = get<double>(newValue);
                } else if (holds_alternative<char>(newValue) && compiler.localsType[name] == TokenType::Char) {
                   it->second->array[index] = get<char>(newValue);
                } else {
                    stringstream ss;
                    ss << "type of local '" << name << "' is incompatible with array " << newValue;
                    Error.report(position, "Runtime", ss.str());
                }
                valueStack.emplace_back(newValue);
                break;

                                          }
            case (OpCode::Pop): {
                if (valueStack.empty()) { break; }
                valueStack.pop_back();
                break;
            }
            case (OpCode::PopLocal): {
                if (valueStack.empty()) { break; }
                locals.erase(get<string>(pop()));
                break;
                                     }
            case (OpCode::Equal): {
                const auto rightOperand = pop();
                if (valueStack.back().index() != rightOperand.index()) {
                    stringstream ss;
                    ss << "Equality between '"  << rightOperand << "' and '" << valueStack.back() << "' cannot be asserted";
                    Error.report(position, "Runtime", ss.str());
                }
                valueStack.back() = valueStack.back() == rightOperand;
                break;
                                  }
            case (OpCode::NotEqual): {
                const auto rightOperand = pop();
                if (valueStack.back().index() != rightOperand.index()) {
                    stringstream ss;
                    ss << "Equality between '"  << rightOperand << "' and '" << valueStack.back() << "' will always result in false";
                    Error.report(position, "Warning", ss.str());
                }
                valueStack.back() = valueStack.back() != rightOperand;
                break;
                                     }
            case (OpCode::Greater): {
                if (isNumber(valueStack.back()) && isNumber(valueStack.crbegin()[1])) {
                    const auto rightOperand = pop();
                    valueStack.back() = valueStack.back() > rightOperand;
                } else if (isType<char>(valueStack.back()) && isType<char>(valueStack.crbegin()[1])) {
                    const auto rightOperand = get<char>(pop());
                    valueStack.back() = get<char>(valueStack.back()) > rightOperand;
                } else {
                    stringstream ss;
                    ss << "use of binary operator '>' between '"  << valueStack.back() << "' and '" << valueStack.crbegin()[1] << "' is not allowed";
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                    }

            case (OpCode::Lesser): {
                if (isNumber(valueStack.back()) && isNumber(valueStack.crbegin()[1])) {
                    const auto rightOperand = pop();
                    valueStack.back() = valueStack.back() < rightOperand;
                } else if (isType<char>(valueStack.back()) && isType<char>(valueStack.crbegin()[1])) {
                    const auto rightOperand = get<char>(pop());
                    valueStack.back() = get<char>(valueStack.back()) < rightOperand;
                } else {
                    stringstream ss;
                    ss << "use of binary operator '<' between '"  << valueStack.back() << "' and '" << valueStack.crbegin()[1] << "' is not allowed";
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                    }


            case (OpCode::LesserEqual): {
                if (isNumber(valueStack.back()) && isNumber(valueStack.crbegin()[1])) {
                    const auto rightOperand = pop();
                    valueStack.back() = valueStack.back() <= rightOperand;
                } else if (isType<char>(valueStack.back()) && isType<char>(valueStack.crbegin()[1])) {


                    const auto rightOperand = get<char>(pop());
                    valueStack.back() = get<char>(valueStack.back()) <= rightOperand;
                } else {
                    stringstream ss;
                    ss << "use of binary operator '<=' between '"  << valueStack.back() << "' and '" << valueStack.crbegin()[1] << "' is not allowed";
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                    }

            case (OpCode::GreaterEqual): {
                if (isNumber(valueStack.back()) && isNumber(valueStack.crbegin()[1])) {
                    const auto rightOperand = pop();
                    valueStack.back() = valueStack.back() >= rightOperand;
                } else if (isType<char>(valueStack.back()) && isType<char>(valueStack.crbegin()[1])) {
                    const auto rightOperand = get<char>(pop());
                    valueStack.back() = get<char>(valueStack.back()) >= rightOperand;
                } else {
                    stringstream ss;
                    ss << "use of binary operator '>=' between '"  << valueStack.back() << "' and '" << valueStack.crbegin()[1] << "' is not allowed";
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                    }

            case (OpCode::Add): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], '+');
                break;
                               }
            case (OpCode::Subtract): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], '-');
                break;
                               }
           case (OpCode::Multiply): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], '*');
                break;
                               }
            case (OpCode::Divide): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], '/');
                break;
                               }
            case (OpCode::Mod): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], '%');
                break;
                               }
            case (OpCode::Div): {
                BinOp(valueStack.back(), valueStack.crbegin()[1], 'd');
                break;
                               }
            case (OpCode::Concatenate): {
                Concatenate(valueStack.back(), valueStack.crbegin()[1]);
                break;
            }
            case (OpCode::Negate): {
               if (isType<i64>(valueStack.back())) {
                    valueStack.push_back(-get<i64>(pop()));
               } else if (isType<double>(valueStack.back())) {
                    valueStack.push_back(-get<double>(pop()));
               } else {
                    stringstream ss;
                    ss << "use of unary operator '-' on '"  << valueStack.back() << "' is not allowed";
                    Error.report(position, "Runtime", ss.str());
               }
               break;
                                     }
            case (OpCode::And): {
                LogicalBinOp(valueStack.back(), valueStack.crbegin()[1], '&');
                break;
                                }
            case (OpCode::Or): {
                LogicalBinOp(valueStack.back(), valueStack.crbegin()[1], '|');
                break;
                                }
            case (OpCode::Not): {
                if (isType<bool>(valueStack.back())) {
                    valueStack.push_back(!get<bool>(pop()));
                } else {
                    stringstream ss;
                    ss << "use of logical unary operator 'NOT' on '"  << valueStack.back() << "' is not allowed. It is only allowed on expressions that evaluate to type Boolean, i.e. NOT(TRUE)";
                    Error.report(position, "Runtime", ss.str());
                }
                break;
                                }
            case (OpCode::Output): {
                cout << Modifier(AnsiCode::FG_BBLACK) << "Output: " << Modifier(AnsiCode::FG_DEFAULT);
                cout << pop() << endl;
                break;
                            }
            case (OpCode::Input): {
                cout << Modifier(AnsiCode::FG_BBLACK) << "Input: " << Modifier(AnsiCode::FG_DEFAULT);
                string input;
                std::getline(std::cin, input);
                input = trim(input);
                if (input[0] == '"' && input.back() == '"') {
                    valueStack.push_back(input.substr(1, input.length() - 1));
                    break;
                } else if (input[0] == '\'' && input[2] == '\'' && input.length() == 3) {
                    valueStack.push_back((input[1]));
                    break;
                } else if (input == "TRUE") {
                    valueStack.push_back(true);
                    break;
                } else if (input == "FALSE") {
                    valueStack.push_back(false);
                    break;
                }
                bool real = false;
                for (const auto c : input) {
                    if (!isdigit(c)) {
                        if (c == '.' && !real) {real = true;}
                        else {Error.report(position, "Runtime", "Unrecognized Input");}
                    }
                }
                if (real){
                    valueStack.push_back((double) std::stod(input));
                } else {
                    valueStack.push_back((i64) std::stoll(input));
                }
                break;
            }
            case (OpCode::Jump): {
                const auto distance = static_cast<size_t>(chunk->read(offset++));
                offset += distance;
                break;
                                 }
            case (OpCode::JumpNE): {
                const auto distance = static_cast<size_t>(chunk->read(offset++));
                if (get<bool>(valueStack.back()) == false) { offset += distance; }
                break;
                                   }
            case (OpCode::Loop): {
                const auto distance = static_cast<size_t>(chunk->read(offset++));
                offset -= distance;
                break;
                                 }
            case (OpCode::IncrementGlobal): {
                const string name = get<string>(pop());
                globals[name] = get<i64>(globals[name]) + 1;
                break;
                                            }
            case (OpCode::Return): {
                return;
                                   }
            default: Error.report(position, "Runtime", "OpCode has no associated functionality\n");
        }
        printValueStack(opCode);
    }
}

void VirtualMachine::printValueStack(OpCode opCode) {
   if (Error.logging == true ) {
       const auto it = OpCodeMap.find(opCode);
       if (it == OpCodeMap.end()) {
           throw std::runtime_error("Invalid argument to printvaluestack");
       }
       std::cout << Modifier(AnsiCode::FG_RED) << "-----------" <<  Modifier(AnsiCode::FG_DEFAULT) <<endl;
       std::cout << it->second + " start: " << std::endl;
       for (auto x : valueStack) {
            cout << "[" << x << "]";
        }
       std::cout << std::endl << it->second + " end" << std::endl;
       std::cout << Modifier(AnsiCode::FG_RED) << "-----------" <<  Modifier(AnsiCode::FG_DEFAULT) <<endl;
    }
}

void VirtualMachine::interpret(string input, int linee) {
    line = linee;
    chunk = compiler.compile(input, line);
    if (Error.logging == true) {
        int step = 1;
        for (auto x : chunk->bytecode) {
            cout << "[" << static_cast<size_t>(x) << "]";
            if (step % 4 == 0) {
                std::cout << std::endl;
            }
            ++step;
        }
        std::cout << "\nConstantPool:\n";
        step = 1;
        for (auto x : chunk->constantPool) {
            cout << "[" << x << "]";
            if (step % 4 == 0) {
                std::cout << std::endl;
            }
            ++step;
        }
    }
    if (chunk == NULL) {
        Error.report(std::pair(0, 0), "Compiler", "Malfunction");
    }
    if (Error.logging) {
        chunk->disassembleChunk("OPCODE");
    }
    run();
}
