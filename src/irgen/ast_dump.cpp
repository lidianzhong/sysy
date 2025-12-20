#include "ast.h"

void CompUnitAST::Dump() const {
  std::cout << "CompUnitAST { ";
  func_def->Dump();
  std::cout << " }";
}

void FuncDefAST::Dump() const {
  std::cout << "FuncDefAST { ";
  func_type->Dump();
  std::cout << ", " << ident << ", ";
  block->Dump();
  std::cout << " }";
}

void FuncTypeAST::Dump() const {
  std::cout << "FuncTypeAST { " << name << " }";
}

void BlockAST::Dump() const {
  std::cout << "BlockAST { ";
  stmt->Dump();
  std::cout << " }";
}

void StmtAST::Dump() const {
  std::cout << "StmtAST { ";
  exp->Dump();
  std::cout << " }";
}

void ExpAST::Dump() const {
  std::cout << "ExpAST { ";
  add_exp->Dump();
  std::cout << " }";
}

void PrimaryExpAST::Dump() const {
  std::cout << "PrimaryExpAST { ";
  if (std::holds_alternative<Number>(data)) {
    std::cout << std::get<Number>(data).val;
  } else if (std::holds_alternative<Exp>(data)) {
    std::get<Exp>(data).ptr->Dump();
  } else {
    throw std::runtime_error("Invalid PrimaryExpAST variant");
  }
  std::cout << " }";
}

void UnaryExpAST::Dump() const {
  std::cout << "UnaryExpAST { ";
  if (std::holds_alternative<Primary>(data)) {
    std::get<Primary>(data).ptr->Dump();
  } else if (std::holds_alternative<Unary>(data)) {
    auto &unary = std::get<Unary>(data);
    std::cout << unary.op << ", ";
    unary.ptr->Dump();
  } else {
    throw std::runtime_error("Invalid UnaryExpAST variant");
  }
  std::cout << " }";
}

void NumberAST::Dump() const { std::cout << "NumberAST { " << val << " }"; }

void MulExpAST::Dump() const {
  std::cout << "MulExpAST { ";
  if (std::holds_alternative<Unary>(data)) {
    std::get<Unary>(data).ptr->Dump();
  } else if (std::holds_alternative<Mul>(data)) {
    auto &mul = std::get<Mul>(data);
    mul.mul_exp->Dump();
    std::cout << ", " << mul.op << ", ";
    mul.unary_exp->Dump();
  } else {
    throw std::runtime_error("Invalid MulExpAST variant");
  }
  std::cout << " }";
}

void AddExpAST::Dump() const {
  std::cout << "AddExpAST { ";
  if (std::holds_alternative<Mul>(data)) {
    std::get<Mul>(data).ptr->Dump();
  } else if (std::holds_alternative<Add>(data)) {
    auto &add = std::get<Add>(data);
    add.add_exp->Dump();
    std::cout << ", " << add.op << ", ";
    add.mul_exp->Dump();
  } else {
    throw std::runtime_error("Invalid AddExpAST variant");
  }
  std::cout << " }";
}