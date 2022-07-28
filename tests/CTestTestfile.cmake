# CMake generated Testfile for 
# Source directory: D:/KMTableLib/tests
# Build directory: D:/KMTableLib/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(table "D:/KMTableLib/bin/test_table")
  set_tests_properties(table PROPERTIES  _BACKTRACE_TRIPLES "D:/KMTableLib/tests/CMakeLists.txt;56;add_test;D:/KMTableLib/tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(table "D:/KMTableLib/bin/test_table")
  set_tests_properties(table PROPERTIES  _BACKTRACE_TRIPLES "D:/KMTableLib/tests/CMakeLists.txt;56;add_test;D:/KMTableLib/tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(table "D:/KMTableLib/bin/test_table")
  set_tests_properties(table PROPERTIES  _BACKTRACE_TRIPLES "D:/KMTableLib/tests/CMakeLists.txt;56;add_test;D:/KMTableLib/tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(table "D:/KMTableLib/bin/test_table")
  set_tests_properties(table PROPERTIES  _BACKTRACE_TRIPLES "D:/KMTableLib/tests/CMakeLists.txt;56;add_test;D:/KMTableLib/tests/CMakeLists.txt;0;")
else()
  add_test(table NOT_AVAILABLE)
endif()
