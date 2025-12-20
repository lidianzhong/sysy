#pragma once

#include "ast.h"
#include "visitor.h"
#include <iostream>
#include <memory>

// RAII 缩进控制
struct IndentGuard {
  int &indent;
  explicit IndentGuard(int &i) : indent(i) { ++indent; }
  ~IndentGuard() { --indent; }
};

class DumpVisitor : public ASTVisitor {
private:
  int indent_level = 0;

  void print_indent() const {
    for (int i = 0; i < indent_level; ++i)
      std::cout << "  ";
  }

  void print_node(const std::string &name) {
    print_indent();
    std::cout << name << std::endl;
  }

public:
  /* =================== 顶层结构 =================== */

  void Visit(CompUnitAST &node) override {
    print_node("CompUnitAST");
    IndentGuard _{indent_level};
    if (node.func_def)
      node.func_def->Accept(*this);
  }

  void Visit(FuncDefAST &node) override {
    print_node("FuncDefAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "RetType: " << node.ret_type << std::endl;

    print_indent();
    std::cout << "Ident: " << node.ident << std::endl;

    if (node.block)
      node.block->Accept(*this);
  }

  void Visit(BlockAST &node) override {
    print_node("BlockAST");
    IndentGuard _{indent_level};
    for (auto &item : node.items)
      if (item)
        item->Accept(*this);
  }

  /* =================== 声明 =================== */

  void Visit(ConstDeclAST &node) override {
    print_node("ConstDeclAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "BType: " << node.btype << std::endl;

    for (auto &def : node.const_defs)
      if (def)
        def->Accept(*this);
  }

  void Visit(ConstDefAST &node) override {
    print_node("ConstDefAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "Ident: " << node.ident << std::endl;

    if (node.init_val)
      node.init_val->Accept(*this);
  }

  void Visit(VarDeclAST &node) override {
    print_node("VarDeclAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "BType: " << node.btype << std::endl;

    for (auto &def : node.var_defs)
      if (def)
        def->Accept(*this);
  }

  void Visit(VarDefAST &node) override {
    print_node("VarDefAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "Ident: " << node.ident << std::endl;

    if (node.init_val)
      node.init_val->Accept(*this);
  }

  /* =================== 语句 =================== */

  void Visit(AssignStmtAST &node) override {
    print_node("AssignStmtAST");
    IndentGuard _{indent_level};

    if (node.lval)
      node.lval->Accept(*this);
    if (node.exp)
      node.exp->Accept(*this);
  }

  void Visit(ReturnStmtAST &node) override {
    print_node("ReturnStmtAST");
    IndentGuard _{indent_level};

    if (node.exp)
      node.exp->Accept(*this);
  }

  /* =================== 表达式 =================== */

  void Visit(LValAST &node) override {
    print_indent();
    std::cout << "LValAST Ident: " << node.ident << std::endl;
  }

  void Visit(NumberAST &node) override {
    print_indent();
    std::cout << "NumberAST Val: " << node.val << std::endl;
  }

  void Visit(UnaryExpAST &node) override {
    print_node("UnaryExpAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "Op: " << node.op << std::endl;

    if (node.exp)
      node.exp->Accept(*this);
  }

  void Visit(BinaryExpAST &node) override {
    print_node("BinaryExpAST");
    IndentGuard _{indent_level};

    print_indent();
    std::cout << "Op: " << node.op << std::endl;

    if (node.lhs)
      node.lhs->Accept(*this);
    if (node.rhs)
      node.rhs->Accept(*this);
  }
};
