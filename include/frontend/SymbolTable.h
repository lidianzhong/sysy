#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

class SymbolTable {
public:
  using Symbol = std::variant<int32_t, std::string>;

  SymbolTable() = default;
  ~SymbolTable() = default;

  void DefineConst(const std::string &name, int32_t value);
  void DefineVar(const std::string &name, const std::string &reg_or_offset);

  std::optional<int32_t> LookupConst(const std::string &name) const;
  std::optional<std::string> LookupVar(const std::string &name) const;

  bool Contains(const std::string &name) const;
  bool IsConstant(const std::string &name) const;
  bool IsVariable(const std::string &name) const;

private:
  std::unordered_map<std::string, Symbol> symbols_;
};