find_package(Qt6 REQUIRED COMPONENTS Test)

# One executable per test file, so a red test names the file it lives in.
# Plain add_executable on purpose: these are console binaries, not app bundles.
function(add_core_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE AscentCore Qt6::Test)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

add_core_test(HashingTest)
add_core_test(CrashCurveTest)
add_core_test(LocalRoundSourceTest)
add_core_test(ProvablyFairTest)
add_core_test(RoundEngineTest)
add_core_test(WalletTest)
