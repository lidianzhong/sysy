#pragma once

#include <string>
#include <unordered_map>

class SymbolTable {
public:
  SymbolTable() = default;
  ~SymbolTable() = default;

  void Insert(const std::string &name, int32_t value);
  int32_t GetValue(const std::string &name) const;
  bool Contains(const std::string &name) const;

private:
  std::unordered_map<std::string, int32_t> symbol_table_;
};