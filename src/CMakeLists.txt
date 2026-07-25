find_package(Qt6 REQUIRED COMPONENTS Core)

# The game logic as one library: the app links it, every test links it, and it is
# compiled once instead of once per target. Nothing in here knows about QML.
add_library(AscentCore STATIC
  CrashCurve.cpp
  LocalRoundSource.cpp
  PlayerStats.cpp
  ProvablyFair.cpp
  RoundEngine.cpp
  RoundHistory.cpp
  RoundSource.cpp
  Wallet.cpp
)

target_link_libraries(AscentCore PUBLIC Qt6::Core)
target_include_directories(AscentCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
