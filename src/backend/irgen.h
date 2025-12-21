#pragma once

#include <sstream>
#include <string>

#include "koopa.h"
#include "visitor.h"

class SymbolTable;

class IRGenVisitor : public ASTVisitor {
public:
  explicit IRGenVisitor(const SymbolTable &symbol_table);

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

  std::string GetIR();
  koopa_raw_program_t GetProgram();

private:
  std::stringstream buffer_;
  std::string last_val_;
  int temp_reg_id_ = 0;
  const SymbolTable &symbol_table_;

  std::string NewTempReg_();

  void VisitCompUnit_(const CompUnitAST *ast);
  void VisitFuncDef_(const FuncDefAST *ast);
  void VisitBlock_(const BlockAST *ast);
  void VisitConstDecl_(const ConstDeclAST *ast);
  void VisitConstDef_(const ConstDefAST *ast);
  void VisitVarDecl_(const VarDeclAST *ast);
  void VisitVarDef_(const VarDefAST *ast);
  void VisitAssignStmt_(const AssignStmtAST *ast);
  void VisitReturnStmt_(const ReturnStmtAST *ast);
  void VisitLVal_(const LValAST *ast);
  void VisitNumber_(const NumberAST *ast);
  void VisitUnaryExp_(const UnaryExpAST *ast);
  void VisitBinaryExp_(const BinaryExpAST *ast);
};