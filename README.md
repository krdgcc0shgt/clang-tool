# Clang AST Study

## DESCRIPTION:

Use libclang to parse a C/C++ program and print the resulting AST.
This is probably only useful to people working on tools based
on libclang.

This implements the sample code in:
https://clang.llvm.org/docs/LibASTMatchersTutorial.html#step-1-create-a-clangtool

## BUILDING:

- Install libclang.
  - work with the latest release and follow the instructions
  from here: https://apt.llvm.org/
- Configure and build clang-ast by running:
  ```
	CC=clang-17 CXX=clang++-17 cmake -S . -B b 
	cmake --build b
  ```
  The previous snippet shows that on MY system I have clang-17
  clang++-17. Modify these as appropriate.

  You can run ``sh rebuildall`` but NOTE that it deletes ``b`` build
  directory prior to executing cmake.

## RUNNING:

```
cd b
./ast path/to/t.cpp
```

## EXAMPLES:

