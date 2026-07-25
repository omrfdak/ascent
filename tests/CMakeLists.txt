find_package(Qt6 REQUIRED COMPONENTS Test)

# One executable per test file, so a red test names the file it lives in.
# Plain add_executable on purpose: these are console binaries, not app bundles.
function(add_core_test name)
  add_executable(${name} ${name}.cpp ${ARGN})
  target_link_libraries(${name} PRIVATE Qt6::Test)
  target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_core_test(HashingTest)
add_core_test(CrashCurveTest ${CMAKE_SOURCE_DIR}/src/CrashCurve.cpp)
