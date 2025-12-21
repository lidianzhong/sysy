#pragma once

#include <optional>

#include "symbol_table.h"
#include "visitor.h"

class BaseAST;

class ConstEvalVisitor : public ASTVisitor {
public:
  void Visit(CompUnitAST &node) override;
  void Visit(FuncDefAST &node) override;
  void Visit(BlockAST &node) override;
  void Visit(ConstDeclAST &node) override;
  void Visit(ConstDefAST &node) override;
  void Visit(VarDeclAST &node) override;
  void Visit(VarDefAST &node) override;
  void Visit(AssignStmtAST &node) override;
  void Visit(ReturnStmtAST &node) override;
  void Visit(LValAST &node) override;
  void Visit(NumberAST &node) override;
  void Visit(UnaryExpAST &node) override;
  void Visit(BinaryExpAST &node) override;

  const SymbolTable &GetSymbolTable() const { return symbol_table_; }
  SymbolTable &GetSymbolTable() { return symbol_table_; }

private:
  void VisitConstDef_(const ConstDefAST &node);
  void VisitLVal_(const LValAST &node);
  void VisitNumber_(const NumberAST &node);
  void VisitUnaryExp_(const UnaryExpAST &node);
  void VisitBinaryExp_(const BinaryExpAST &node);

private:
  SymbolTable symbol_table_;
  std::optional<int32_t> last_val_ = 0;
};