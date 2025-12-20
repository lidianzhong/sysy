#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "ast.h"
#include "backend/irgen.h"
#include "backend/riscvgen.h"
#include "koopa.h"
#include "utils/dump_visitor.h"

using namespace std;

class DumpVisitor;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  assert(argc == 5);

  string mode(argv[1]);  // 模式: -koopa or -riscv
  auto input = argv[2];  // 输入文件
  auto output = argv[4]; // 输出文件

  if (mode != "-koopa" && mode != "-riscv") {
    cerr << "Error: Unsupported mode " << mode << endl;
    return 1;
  }

  // open source file
  yyin = fopen(input, "r");
  assert(yyin);

  // parse
  unique_ptr<BaseAST> ast;
  auto parse_ret = yyparse(ast);
  assert(!parse_ret);

  // print AST
  // DumpVisitor dumper;
  // ast->Accept(dumper);

  // AST -> Koopa IR
  IRGenVisitor ir_gen;
  ast->Accept(ir_gen);
  std::string ir = ir_gen.GetIR();

  // Koopa IR -> RISC-V
  koopa_raw_program_t raw = ir_gen.GetProgram();

  if (std::string(mode) == "-koopa") {
    FILE *out = fopen(output, "w");
    assert(out);
    fprintf(out, "%s", ir.c_str());
    fclose(out);
  } else if (std::string(mode) == "-riscv") {
    freopen(output, "w", stdout);
    Visit(raw);
    fclose(stdout);
  }

  return 0;
}