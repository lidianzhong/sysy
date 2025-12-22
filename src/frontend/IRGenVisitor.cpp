#include <cassert>
#include <memory>

#include "frontend/IRGenVisitor.h"

#include "frontend/AST.h"
#include "frontend/SymbolTable.h"
#include "ir/IRBuilder.h"

IRGenVisitor::IRGenVisitor(SymbolTable &symtab)
    : symtab_(symtab), builder_(std::make_unique<IRBuilder>()) {}

std::string IRGenVisitor::GetIR() const {
  assert(builder_);
  return builder_->GetIR();
}

koopa_raw_program_t IRGenVisitor::GetProgram() const {
  assert(builder_);
  return builder_->Build();
}

void IRGenVisitor::VisitCompUnit_(const CompUnitAST *ast) {
  if (ast->func_def) {
    ast->func_def->Accept(*this);
  }
}

void IRGenVisitor::VisitFuncDef_(const FuncDefAST *ast) {
  assert(builder_);
  std::string ret_type = ast->ret_type == "int" ? "i32" : "void";
  builder_->StartFunction(ast->ident, ret_type);
  builder_->CreateBasicBlock("entry");
  if (ast->block) {
    ast->block->Accept(*this);
  }
  builder_->EndFunction();
}

void IRGenVisitor::VisitBlock_(const BlockAST *ast) {
  for (auto &item : ast->items) {
    if (item) {
      item->Accept(*this);
    }
  }
}

void IRGenVisitor::VisitConstDecl_(const ConstDeclAST *ast) {
  for (auto &def : ast->const_defs) {
    if (def) {
      def->Accept(*this);
    }
  }
}

void IRGenVisitor::VisitConstDef_(const ConstDefAST *ast) {}

void IRGenVisitor::VisitVarDecl_(const VarDeclAST *ast) {
  for (auto &def : ast->var_defs) {
    if (def) {
      def->Accept(*this);
    }
  }
}

void IRGenVisitor::VisitVarDef_(const VarDefAST *ast) {
  assert(builder_);
  std::string alloc_addr = builder_->CreateAlloca("i32");
  symtab_.DefineVar(ast->ident, alloc_addr);

  if (ast->init_val) {
    ast->init_val->Accept(*this);
  } else {
    last_val_ = "0"; // 变量未初始化填0
  }

  std::string init_value = last_val_;
  builder_->CreateStore(init_value, alloc_addr);
}

void IRGenVisitor::VisitAssignStmt_(const AssignStmtAST *ast) {
  if (!ast->lval || !ast->exp) {
    std::cerr << "AssignStmtAST: lval or exp is null" << std::endl;
    return;
  }

  // 获取左侧变量的地址（直接从符号表查找，不通过 VisitLVal_）
  const LValAST *lval_ast = dynamic_cast<const LValAST *>(ast->lval.get());
  if (!lval_ast) {
    return;
  }

  std::string var_alloc;
  if (symtab_.IsVariable(lval_ast->ident)) {
    auto alloc_opt = symtab_.LookupVar(lval_ast->ident);
    if (alloc_opt.has_value()) {
      var_alloc = alloc_opt.value();
    } else {
      return; // 变量未定义
    }
  } else {
    return; // 不能对常量赋值
  }

  // 计算右侧表达式的值
  ast->exp->Accept(*this);
  std::string exp_val = last_val_;

  // 生成 store 指令
  assert(builder_);
  builder_->CreateStore(exp_val, var_alloc);
}

void IRGenVisitor::VisitReturnStmt_(const ReturnStmtAST *ast) {
  assert(builder_);
  if (ast->exp) {
    ast->exp->Accept(*this);
    builder_->CreateReturn(last_val_);
  } else {
    builder_->CreateReturn();
  }
}

void IRGenVisitor::VisitLVal_(const LValAST *ast) {
  // 检查是否是常量
  if (symtab_.IsConstant(ast->ident)) {
    auto const_val = symtab_.LookupConst(ast->ident);
    if (const_val.has_value()) {
      last_val_ = std::to_string(const_val.value());
    } else {
      last_val_ = "0";
    }
  } else if (symtab_.IsVariable(ast->ident)) {
    // 是变量：生成 load 指令获取值
    assert(builder_);
    auto var_alloc = symtab_.LookupVar(ast->ident);
    if (var_alloc.has_value()) {
      last_val_ = builder_->CreateLoad(var_alloc.value());
    } else {
      last_val_ = "0";
    }
  } else {
    last_val_ = "0";
  }
}

void IRGenVisitor::VisitNumber_(const NumberAST *ast) {
  assert(builder_);
  last_val_ = builder_->CreateNumber(ast->val);
}

void IRGenVisitor::VisitUnaryExp_(const UnaryExpAST *ast) {
  assert(builder_);
  if (ast->exp) {
    ast->exp->Accept(*this);
    std::string operand = last_val_;
    last_val_ = builder_->CreateUnaryOp(ast->op, operand);
  }
}

void IRGenVisitor::VisitBinaryExp_(const BinaryExpAST *ast) {
  if (!ast->lhs || !ast->rhs) {
    std::cerr << "BinaryExpAST: lhs or rhs is null" << std::endl;
    return;
  }

  ast->lhs->Accept(*this);
  std::string lhs_val = last_val_;

  ast->rhs->Accept(*this);
  std::string rhs_val = last_val_;

  assert(builder_);
  last_val_ = builder_->CreateBinaryOp(ast->op, lhs_val, rhs_val);
}

void IRGenVisitor::Visit(CompUnitAST &node) { VisitCompUnit_(&node); }
void IRGenVisitor::Visit(FuncDefAST &node) { VisitFuncDef_(&node); }
void IRGenVisitor::Visit(BlockAST &node) { VisitBlock_(&node); }
void IRGenVisitor::Visit(ConstDeclAST &node) { VisitConstDecl_(&node); }
void IRGenVisitor::Visit(ConstDefAST &node) { VisitConstDef_(&node); }
void IRGenVisitor::Visit(VarDeclAST &node) { VisitVarDecl_(&node); }
void IRGenVisitor::Visit(VarDefAST &node) { VisitVarDef_(&node); }
void IRGenVisitor::Visit(AssignStmtAST &node) { VisitAssignStmt_(&node); }
void IRGenVisitor::Visit(ReturnStmtAST &node) { VisitReturnStmt_(&node); }
void IRGenVisitor::Visit(LValAST &node) { VisitLVal_(&node); }
void IRGenVisitor::Visit(NumberAST &node) { VisitNumber_(&node); }
void IRGenVisitor::Visit(UnaryExpAST &node) { VisitUnaryExp_(&node); }
void IRGenVisitor::Visit(BinaryExpAST &node) { VisitBinaryExp_(&node); }
