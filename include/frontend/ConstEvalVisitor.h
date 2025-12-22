#pragma once

#include <optional>

#include "ASTVisitor.h"
#include "SymbolTable.h"

class BaseAST;
class SymbolTable;

class ConstEvalVisitor : public ASTVisitor {
public:
  explicit ConstEvalVisitor(SymbolTable &symtab);

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

private:
  void VisitConstDef_(const ConstDefAST &node);
  void VisitLVal_(const LValAST &node);
  void VisitNumber_(const NumberAST &node);
  void VisitUnaryExp_(const UnaryExpAST &node);
  void VisitBinaryExp_(const BinaryExpAST &node);

private:
  SymbolTable &symtab_;
  std::optional<int32_t> last_val_;
};