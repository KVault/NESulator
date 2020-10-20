# CMake generated Testfile for 
# Source directory: /Users/jose/Developer/src/github.com/NESulator
# Build directory: /Users/jose/Developer/src/github.com/NESulator/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Testulator "test/TestMain.c")
set_tests_properties(Testulator PROPERTIES  _BACKTRACE_TRIPLES "/Users/jose/Developer/src/github.com/NESulator/CMakeLists.txt;9;add_test;/Users/jose/Developer/src/github.com/NESulator/CMakeLists.txt;0;")
subdirs("src")
subdirs("test")
subdirs("gui_runtime")
