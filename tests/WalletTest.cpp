#include <QtTest>

#include "Wallet.h"

class WalletTest : public QObject
{
  Q_OBJECT

private slots:
  void startsWithTheGivenBalance();
  void rejectsBetsOutsideTheLimits();
  void rejectsABetTheBalanceCannotCover();
  void placingABetTakesExactlyTheBet();
  void payOutIsTheBetTimesTheMultiplier();
  void cashingOutImmediatelyReturnsTheBet();
  void repeatedRoundsDoNotDriftAPoint();
  void roundsHalfwayAmountsUp();
  void payOutRefusesAnImpossibleRound();
  void bailOutOnlyRescuesABrokePlayer();
};

void WalletTest::startsWithTheGivenBalance()
{
  const Wallet wallet(1000);

  QCOMPARE(wallet.balance(), 1000.0);
  QCOMPARE(wallet.minimumBet(), 25.0);
  QCOMPARE(wallet.maximumBet(), 2500.0);
}

void WalletTest::rejectsBetsOutsideTheLimits()
{
  const Wallet wallet(100000);

  QVERIFY(!wallet.canPlaceBet(24));
  QVERIFY(!wallet.canPlaceBet(0));
  QVERIFY(!wallet.canPlaceBet(-100));
  QVERIFY(!wallet.canPlaceBet(2501));

  // The limits themselves are allowed - an off by one here is a real bug.
  QVERIFY(wallet.canPlaceBet(25));
  QVERIFY(wallet.canPlaceBet(2500));
}

void WalletTest::rejectsABetTheBalanceCannotCover()
{
  Wallet wallet(100);

  QVERIFY(!wallet.canPlaceBet(101));
  QVERIFY(!wallet.placeBet(101));
  QCOMPARE(wallet.balance(), 100.0); // a refused bet must not touch the balance

  // Betting everything is fine, betting a point more is not.
  QVERIFY(wallet.placeBet(100));
  QCOMPARE(wallet.balance(), 0.0);
}

void WalletTest::placingABetTakesExactlyTheBet()
{
  Wallet wallet(1000);
  QSignalSpy spy(&wallet, &Wallet::balanceChanged);

  QVERIFY(wallet.placeBet(250));

  QCOMPARE(wallet.balance(), 750.0);
  QCOMPARE(spy.count(), 1);
}

void WalletTest::payOutIsTheBetTimesTheMultiplier()
{
  Wallet wallet(1000);

  QVERIFY(wallet.placeBet(100));
  QCOMPARE(wallet.payOut(100, 2.37), 237.0);
  QCOMPARE(wallet.balance(), 1137.0);

  // Fractions survive: a multiplier has two decimals, so a payout does too.
  Wallet other(1000);
  QCOMPARE(other.payOut(25, 1.13), 28.25);
  QCOMPARE(other.payOut(33, 1.51), 49.83);
}

void WalletTest::cashingOutImmediatelyReturnsTheBet()
{
  Wallet wallet(1000);

  QVERIFY(wallet.placeBet(500));
  QCOMPARE(wallet.payOut(500, 1.0), 500.0);
  QCOMPARE(wallet.balance(), 1000.0);
}

void WalletTest::repeatedRoundsDoNotDriftAPoint()
{
  Wallet wallet(1000);

  // Ten thousand rounds whose payouts do not land on whole points. Left to raw
  // floating point the remainders would pile up and the balance would end on
  // something like 999.9999999998; snapped to hundredths it cannot.
  for (int i = 0; i < 10000; ++i) {
    QVERIFY(wallet.placeBet(100));
    wallet.payOut(100, 1.13);
  }

  QCOMPARE(wallet.balance(), 1000.0 + 10000 * 13.0);
}

void WalletTest::roundsHalfwayAmountsUp()
{
  Wallet wallet(0);

  // Two decimals, halfway goes up. Worth pinning because 25.655 * 100 is not
  // 2565.5 in binary but a hair below it, which would round the wrong way.
  QCOMPARE(wallet.payOut(25.655, 1.0), 25.66);
  QCOMPARE(wallet.payOut(25.654, 1.0), 25.65);
  QCOMPARE(wallet.payOut(25.657, 1.0), 25.66);
}

void WalletTest::payOutRefusesAnImpossibleRound()
{
  Wallet wallet(1000);

  // Below 1.00x there is no cash out to pay - the round popped.
  QCOMPARE(wallet.payOut(100, 0.5), 0.0);
  QCOMPARE(wallet.payOut(0, 5.0), 0.0);
  QCOMPARE(wallet.payOut(-100, 5.0), 0.0);
  QCOMPARE(wallet.balance(), 1000.0);
}

void WalletTest::bailOutOnlyRescuesABrokePlayer()
{
  Wallet solvent(25);
  QVERIFY(!solvent.bailOut()); // can still afford the smallest bet
  QCOMPARE(solvent.balance(), 25.0);

  Wallet broke(24.99);
  QVERIFY(broke.bailOut());
  QCOMPARE(broke.balance(), 500.0);

  // And it cannot be farmed once the player is back on their feet.
  QVERIFY(!broke.bailOut());
}

QTEST_APPLESS_MAIN(WalletTest)

#include "WalletTest.moc"
