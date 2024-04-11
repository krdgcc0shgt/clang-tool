// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang;
using namespace clang::ast_matchers;

StatementMatcher LoopMatcher =
  forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(
    hasInitializer(integerLiteral(equals(0)))))))).bind("forLoop");

class LoopPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const ForStmt *FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
      FS->dump();
  }
};

/* Binding for "root": */
/* CXXDeleteExpr 0x562ae1b3aeb8 </home/rgc/t/clang-ast/t.cpp:22:3, col:11> 'void' Function 0x562ae1b36560 'operator delete' 'void (void *) noexcept' */
/* `-ImplicitCastExpr 0x562ae1b3aea0 <col:10, col:11> 'int *' <LValueToRValue> */
/*   `-UnaryOperator 0x562ae1b3ae88 <col:10, col:11> 'int *' lvalue prefix '*' cannot overflow */
/*     `-ImplicitCastExpr 0x562ae1b3ae70 <col:11> 'int **' <LValueToRValue> */
/*       `-DeclRefExpr 0x562ae1b3ae50 <col:11> 'int **' lvalue Var 0x562ae1b3ad68 'pp' 'int **' */

/* Binding for "varName": */
/* DeclRefExpr 0x562ae1b3ae50 </home/rgc/t/clang-ast/t.cpp:22:11> 'int **' lvalue Var 0x562ae1b3ad68 'pp' 'int **' */

StatementMatcher DeleteMatcher =
  cxxDeleteExpr(hasDescendant(declRefExpr().bind("varName")));

class DeletePrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const auto *delete_stmt = Result.Nodes.getNodeAs<DeclRefExpr>("varName")) {
      const auto *valueDecl = delete_stmt->getDecl();
      llvm::outs() << "Variable name used in delete: " << valueDecl->getNameAsString() << "\n";
    }
  }
};

/* m binaryOperator(hasOperatorName("=="), hasRHS(hasDescendant(declRefExpr(to(varDecl(hasName("pp"))))))) */
/* Binding for "root": */
/* BinaryOperator 0x562ae1b3afa0 </home/rgc/t/clang-ast/t.cpp:23:7, col:14> 'bool' '==' */
/* |-UnaryOperator 0x562ae1b3aef8 <col:7, col:8> 'int *' prefix '&' cannot overflow */
/* | `-DeclRefExpr 0x562ae1b3aed8 <col:8> 'int' lvalue Var 0x562ae1b1b6f0 'x' 'int' */
/* `-ImplicitCastExpr 0x562ae1b3af88 <col:13, col:14> 'int *' <LValueToRValue> */
/*   `-UnaryOperator 0x562ae1b3af70 <col:13, col:14> 'int *' lvalue prefix '*' cannot overflow */
/*     `-ImplicitCastExpr 0x562ae1b3af58 <col:14> 'int **' <LValueToRValue> */
/*       `-DeclRefExpr 0x562ae1b3af38 <col:14> 'int **' lvalue Var 0x562ae1b3ad68 'pp' 'int **' */

/* m ifStmt(hasCondition(expr(binaryOperator(hasOperatorName("=="), hasRHS(hasDescendant(declRefExpr())))))) */
/* m ifStmt(hasCondition(expr(binaryOperator(hasOperatorName("=="), hasRHS(hasDescendant(declRefExpr().bind("var"))))))) */
/* Match #1: */

/* /home/rgc/t/clang-ast/t.cpp:23:3: note: "root" binds here */
/*    23 |   if (&x == *pp) */
/*       |   ^~~~~~~~~~~~~~ */
/*    24 |     offset2(x, x); */
/*       |     ~~~~~~~~~~~~~ */
/* /home/rgc/t/clang-ast/t.cpp:23:14: note: "var" binds here */
/*    23 |   if (&x == *pp) */
/*       |              ^~ */
/* 1 match. */

StatementMatcher IfMatcher =
  ifStmt(hasCondition(expr(binaryOperator(hasOperatorName("=="), hasRHS(hasDescendant(declRefExpr().bind("varName")))))));

class IfPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const auto *if_stmt = Result.Nodes.getNodeAs<DeclRefExpr>("varName")) {
      const auto *valueDecl = if_stmt->getDecl();
      llvm::outs() << "Variable name used in if comparision: " << valueDecl->getNameAsString() << "\n";
    }
  }
};

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
  auto ExpectedParser = CommonOptionsParser::create(argc, argv, MyToolCategory);
  if (!ExpectedParser) {
    // Fail gracefully for unsupported options.
    llvm::errs() << ExpectedParser.takeError();
    return 1;
  }
  CommonOptionsParser& OptionsParser = ExpectedParser.get();
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  LoopPrinter Printer;
  MatchFinder Finder;
  Finder.addMatcher(LoopMatcher, &Printer);

  DeletePrinter Delete_Printer;
  Finder.addMatcher(DeleteMatcher, &Delete_Printer);

  IfPrinter If_Printer;
  Finder.addMatcher(IfMatcher, &If_Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}
