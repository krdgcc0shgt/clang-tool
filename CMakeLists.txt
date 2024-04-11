cmake_minimum_required(VERSION 3.16)

project(clang-ast)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${LLVM_CMAKE_DIR}"
  "${CLANG_CMAKE_DIR}"
  )

include(AddLLVM)
include(AddClang)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM CMake at : ${LLVM_CMAKE_DIR}")
message(STATUS "Clang CMake at : ${CLANG_CMAKE_DIR}")
message(STATUS "LLVM LIBS : ${LLVM_LIBS}")
message(STATUS "Clang LIBS : ${CLANG_LIBS}")

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})

add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

add_executable(ast ast.cpp)
# target_link_libraries(ast clang LLVM-17)
target_link_libraries(ast
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
)
