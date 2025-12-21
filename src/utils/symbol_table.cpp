#include <cstdint>
#include <stdexcept>

#include "symbol_table.h"

void SymbolTable::Insert(const std::string &name, int32_t value) {
  symbol_table_[name] = value;
}

int32_t SymbolTable::GetValue(const std::string &name) const {
  auto it = symbol_table_.find(name);
  if (it != symbol_table_.end()) {
    return it->second;
  }
  throw std::runtime_error("Symbol not found: " + name);
}

bool SymbolTable::Contains(const std::string &name) const {
  return symbol_table_.find(name) != symbol_table_.end();
}
