#include <cstdint>
#include <stdexcept>

#include "frontend/SymbolTable.h"

void SymbolTable::DefineConst(const std::string &name, int32_t value) {
  symbols_[name] = value;
}

void SymbolTable::DefineVar(const std::string &name, const std::string &alloc) {
  symbols_[name] = alloc;
}

std::optional<int32_t> SymbolTable::LookupConst(const std::string &name) const {
  auto it = symbols_.find(name);
  if (it != symbols_.end()) {
    if (std::holds_alternative<int32_t>(it->second)) {
      return std::get<int32_t>(it->second);
    }
    throw std::runtime_error("Symbol is not a constant: " + name);
  }
  throw std::runtime_error("Symbol not found: " + name);
}

std::optional<std::string>
SymbolTable::LookupVar(const std::string &name) const {
  auto it = symbols_.find(name);
  if (it != symbols_.end()) {
    if (std::holds_alternative<std::string>(it->second)) {
      return std::get<std::string>(it->second);
    }
    throw std::runtime_error("Symbol is not a variable: " + name);
  }
  throw std::runtime_error("Symbol not found: " + name);
}

bool SymbolTable::Contains(const std::string &name) const {
  return symbols_.find(name) != symbols_.end();
}

bool SymbolTable::IsConstant(const std::string &name) const {
  auto it = symbols_.find(name);
  if (it != symbols_.end()) {
    return std::holds_alternative<int32_t>(it->second);
  }
  return false;
}

bool SymbolTable::IsVariable(const std::string &name) const {
  auto it = symbols_.find(name);
  if (it != symbols_.end()) {
    return std::holds_alternative<std::string>(it->second);
  }
  return false;
}
