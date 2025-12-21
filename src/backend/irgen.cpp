#include <cassert>

#include "ast.h"
#include "irgen.h"
#include "utils/symbol_table.h"

IRGenVisitor::IRGenVisitor(const SymbolTable &symbol_table)
    : symbol_table_(symbol_table) {}

std::string IRGenVisitor::GetIR() { return buffer_.str(); }

koopa_raw_program_t IRGenVisitor::GetProgram() {
  koopa_program_t program = nullptr;
  std::string ir = buffer_.str();
  koopa_error_code_t ret = koopa_parse_from_string(ir.c_str(), &program);
  assert(ret == KOOPA_EC_SUCCESS);

  auto builder = koopa_new_raw_program_builder();
  return koopa_build_raw_program(builder, program);
}

std::string IRGenVisitor::NewTempReg_() {
  return "%" + std::to_string(temp_reg_id_++);
}

void IRGenVisitor::VisitCompUnit_(const CompUnitAST *ast) {
  if (ast->func_def) {
    ast->func_def->Accept(*this);
  }
}

void IRGenVisitor::VisitFuncDef_(const FuncDefAST *ast) {
  buffer_ << "fun @" << ast->ident << "(): ";
  if (ast->ret_type == "int") {
    buffer_ << "i32";
  }
  buffer_ << " {" << std::endl;
  buffer_ << "%entry:" << std::endl;
  if (ast->block) {
    ast->block->Accept(*this);
  }
  buffer_ << "}" << std::endl;
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

void IRGenVisitor::VisitConstDef_(const ConstDefAST *ast) {
  // 常量定义：在编译期已经由 ConstEvalVisitor 处理并存入符号表
  // 这里不需要生成 IR，因为常量在编译期已经求值完成
  // 如果 init_val 中包含常量引用，会在 VisitLVal_ 中从符号表查询
}

void IRGenVisitor::VisitVarDecl_(const VarDeclAST *ast) {
  for (auto &def : ast->var_defs) {
    if (def) {
      def->Accept(*this);
    }
  }
}

void IRGenVisitor::VisitVarDef_(const VarDefAST *ast) {
  // 变量定义：如果有初始值，计算并存储
  if (ast->init_val) {
    ast->init_val->Accept(*this);
    // TODO: 存储变量值（如果需要）
  }
}

void IRGenVisitor::VisitAssignStmt_(const AssignStmtAST *ast) {
  if (!ast->lval || !ast->exp) {
    return;
  }

  // 先获取左侧变量名
  ast->lval->Accept(*this);
  std::string var_name = last_val_;

  // 计算右侧表达式的值
  ast->exp->Accept(*this);
  std::string exp_val = last_val_;

  // 生成赋值指令（store 指令：store 值, 地址）
  buffer_ << "  store " << exp_val << ", @" << var_name << std::endl;
}

void IRGenVisitor::VisitReturnStmt_(const ReturnStmtAST *ast) {
  if (ast->exp) {
    ast->exp->Accept(*this);
    buffer_ << "  ret " << last_val_ << std::endl;
  } else {
    buffer_ << "  ret" << std::endl;
  }
}

void IRGenVisitor::VisitLVal_(const LValAST *ast) {
  // 先检查是否是常量
  if (symbol_table_.Contains(ast->ident)) {
    // 是常量：直接返回常量值（作为立即数）
    int32_t const_val = symbol_table_.GetValue(ast->ident);
    last_val_ = std::to_string(const_val);
  } else {
    // 是变量：返回变量名（用于后续使用）
    last_val_ = ast->ident;
  }
}

void IRGenVisitor::VisitNumber_(const NumberAST *ast) {
  last_val_ = std::to_string(ast->val);
}

void IRGenVisitor::VisitUnaryExp_(const UnaryExpAST *ast) {
  if (ast->exp) {
    ast->exp->Accept(*this);
    std::string operand = last_val_;

    std::string temp_reg = NewTempReg_();
    if (ast->op == "-") {
      buffer_ << "  " << temp_reg << " = sub 0, " << operand << std::endl;
    } else if (ast->op == "!") {
      buffer_ << "  " << temp_reg << " = eq " << operand << ", 0" << std::endl;
    } else {
      // "+" 操作，直接使用操作数
      last_val_ = operand;
      return;
    }
    last_val_ = temp_reg;
  }
}

void IRGenVisitor::VisitBinaryExp_(const BinaryExpAST *ast) {
  if (!ast->lhs || !ast->rhs) {
    // 如果缺少操作数，返回默认值
    last_val_ = "0";
    return;
  }

  // 访问左操作数
  ast->lhs->Accept(*this);
  std::string lhs_val = last_val_;

  // 访问右操作数
  ast->rhs->Accept(*this);
  std::string rhs_val = last_val_;

  std::string temp_reg = NewTempReg_();
  if (ast->op == "+") {
    buffer_ << "  " << temp_reg << " = add " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "-") {
    buffer_ << "  " << temp_reg << " = sub " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "*") {
    buffer_ << "  " << temp_reg << " = mul " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "/") {
    buffer_ << "  " << temp_reg << " = div " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "%") {
    buffer_ << "  " << temp_reg << " = mod " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "<") {
    buffer_ << "  " << temp_reg << " = lt " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == ">") {
    buffer_ << "  " << temp_reg << " = gt " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "<=") {
    buffer_ << "  " << temp_reg << " = le " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == ">=") {
    buffer_ << "  " << temp_reg << " = ge " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "==") {
    buffer_ << "  " << temp_reg << " = eq " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "!=") {
    buffer_ << "  " << temp_reg << " = ne " << lhs_val << ", " << rhs_val
            << std::endl;
  } else if (ast->op == "&&") {
    // 逻辑与：短路求值需要分支，这里简化处理
    std::string temp1 = NewTempReg_();
    buffer_ << "  " << temp1 << " = ne " << lhs_val << ", 0" << std::endl;
    std::string temp2 = NewTempReg_();
    buffer_ << "  " << temp2 << " = ne " << rhs_val << ", 0" << std::endl;
    buffer_ << "  " << temp_reg << " = and " << temp1 << ", " << temp2
            << std::endl;
  } else if (ast->op == "||") {
    // 逻辑或：短路求值需要分支，这里简化处理
    std::string temp1 = NewTempReg_();
    buffer_ << "  " << temp1 << " = ne " << lhs_val << ", 0" << std::endl;
    std::string temp2 = NewTempReg_();
    buffer_ << "  " << temp2 << " = ne " << rhs_val << ", 0" << std::endl;
    buffer_ << "  " << temp_reg << " = or " << temp1 << ", " << temp2
            << std::endl;
  }
  last_val_ = temp_reg;
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
