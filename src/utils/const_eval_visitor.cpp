#include <cassert>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>

#include "ast.h"
#include "const_eval_visitor.h"

void ConstEvalVisitor::VisitConstDef_(const ConstDefAST &node) {
  if (node.init_val) {
    node.init_val->Accept(*this);
    assert(last_val_.has_value()); // init_val 必须是常量表达式

    // 将常量加入符号表
    int32_t value = last_val_.value();
    symbol_table_.Insert(node.ident, value);
  }
}

void ConstEvalVisitor::VisitLVal_(const LValAST &node) {
  // 从符号表中获取常量值
  assert(symbol_table_.Contains(node.ident));
  last_val_ = symbol_table_.GetValue(node.ident);
}

void ConstEvalVisitor::VisitNumber_(const NumberAST &node) {
  last_val_ = node.val;
}

void ConstEvalVisitor::VisitUnaryExp_(const UnaryExpAST &node) {
  if (node.exp) {
    node.exp->Accept(*this);

    if (!last_val_.has_value()) {
      last_val_ = std::nullopt;
      return; // 非常量，无法求值
    }

    int32_t operand = last_val_.value();
    if (node.op == "+") {
      last_val_ = operand;
    } else if (node.op == "-") {
      last_val_ = -operand;
    } else if (node.op == "!") {
      last_val_ = operand == 0 ? 1 : 0;
    }
  }
}

void ConstEvalVisitor::VisitBinaryExp_(const BinaryExpAST &node) {
  assert(node.lhs && node.rhs);

  node.lhs->Accept(*this);
  std::optional<int32_t> lhs_val_opt = last_val_;

  node.rhs->Accept(*this);
  std::optional<int32_t> rhs_val_opt = last_val_;

  if (!lhs_val_opt.has_value() || !rhs_val_opt.has_value()) {
    last_val_ = std::nullopt;
    return; // 非常量，无法求值
  }

  int32_t lhs_val = lhs_val_opt.value();
  int32_t rhs_val = rhs_val_opt.value();

  // 应用二元运算符
  if (node.op == "+") {
    last_val_ = lhs_val + rhs_val;
  } else if (node.op == "-") {
    last_val_ = lhs_val - rhs_val;
  } else if (node.op == "*") {
    last_val_ = lhs_val * rhs_val;
  } else if (node.op == "/") {
    if (rhs_val == 0) {
      throw std::runtime_error("Division by zero");
    }
    last_val_ = lhs_val / rhs_val;
  } else if (node.op == "%") {
    if (rhs_val == 0) {
      throw std::runtime_error("Modulo by zero");
    }
    last_val_ = lhs_val % rhs_val;
  } else if (node.op == "<") {
    last_val_ = lhs_val < rhs_val ? 1 : 0;
  } else if (node.op == ">") {
    last_val_ = lhs_val > rhs_val ? 1 : 0;
  } else if (node.op == "<=") {
    last_val_ = lhs_val <= rhs_val ? 1 : 0;
  } else if (node.op == ">=") {
    last_val_ = lhs_val >= rhs_val ? 1 : 0;
  } else if (node.op == "==") {
    last_val_ = lhs_val == rhs_val ? 1 : 0;
  } else if (node.op == "!=") {
    last_val_ = lhs_val != rhs_val ? 1 : 0;
  } else if (node.op == "&&") {
    last_val_ = (lhs_val != 0 && rhs_val != 0) ? 1 : 0;
  } else if (node.op == "||") {
    last_val_ = (lhs_val != 0 || rhs_val != 0) ? 1 : 0;
  }
}

void ConstEvalVisitor::Visit(CompUnitAST &node) {
  node.func_def->Accept(*this);
}
void ConstEvalVisitor::Visit(FuncDefAST &node) { node.block->Accept(*this); }
void ConstEvalVisitor::Visit(BlockAST &node) {
  for (auto &item : node.items) {
    item->Accept(*this);
  }
}
void ConstEvalVisitor::Visit(ConstDeclAST &node) {
  for (auto &def : node.const_defs) {
    def->Accept(*this);
  }
}
void ConstEvalVisitor::Visit(ConstDefAST &node) { VisitConstDef_(node); }
void ConstEvalVisitor::Visit(VarDeclAST &node) {}
void ConstEvalVisitor::Visit(VarDefAST &node) {}
void ConstEvalVisitor::Visit(AssignStmtAST &node) {}
void ConstEvalVisitor::Visit(ReturnStmtAST &node) { node.exp->Accept(*this); }
void ConstEvalVisitor::Visit(LValAST &node) { VisitLVal_(node); }
void ConstEvalVisitor::Visit(NumberAST &node) { VisitNumber_(node); }
void ConstEvalVisitor::Visit(UnaryExpAST &node) { VisitUnaryExp_(node); }
void ConstEvalVisitor::Visit(BinaryExpAST &node) { VisitBinaryExp_(node); }