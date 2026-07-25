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
  void payOutRefusesAnImpossibleRound();
  void bailOutOnlyRescuesABrokePlayer();
};

void WalletTest::startsWithTheGivenBalance()
{
  const Wallet wallet(1000);

  QCOMPARE(wallet.balance(), 1000);
  QCOMPARE(wallet.minimumBet(), 25);
  QCOMPARE(wallet.maximumBet(), 2500);
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
  QCOMPARE(wallet.balance(), 100); // a refused bet must not touch the balance

  // Betting everything is fine, betting a point more is not.
  QVERIFY(wallet.placeBet(100));
  QCOMPARE(wallet.balance(), 0);
}

void WalletTest::placingABetTakesExactlyTheBet()
{
  Wallet wallet(1000);
  QSignalSpy spy(&wallet, &Wallet::balanceChanged);

  QVERIFY(wallet.placeBet(250));

  QCOMPARE(wallet.balance(), 750);
  QCOMPARE(spy.count(), 1);
}

void WalletTest::payOutIsTheBetTimesTheMultiplier()
{
  Wallet wallet(1000);

  QVERIFY(wallet.placeBet(100));
  QCOMPARE(wallet.payOut(100, 2.37), 237);
  QCOMPARE(wallet.balance(), 1137);

  // A payout that does not divide evenly is truncated, never rounded up: the
  // player can never be credited a point the multiplier did not earn.
  Wallet other(1000);
  QCOMPARE(other.payOut(25, 1.03), 25); // 25 * 1.03 = 25.75
  QCOMPARE(other.payOut(33, 1.51), 49); // 33 * 1.51 = 49.83
}

void WalletTest::cashingOutImmediatelyReturnsTheBet()
{
  Wallet wallet(1000);

  QVERIFY(wallet.placeBet(500));
  QCOMPARE(wallet.payOut(500, 1.0), 500);
  QCOMPARE(wallet.balance(), 1000);
}

void WalletTest::repeatedRoundsDoNotDriftAPoint()
{
  Wallet wallet(1000);

  // Ten thousand rounds that each break even. With a floating point balance the
  // remainder of 1.07 would quietly accumulate; with whole points it cannot.
  for (int i = 0; i < 10000; ++i) {
    QVERIFY(wallet.placeBet(100));
    wallet.payOut(100, 1.0);
  }

  QCOMPARE(wallet.balance(), 1000);
}

void WalletTest::payOutRefusesAnImpossibleRound()
{
  Wallet wallet(1000);

  // Below 1.00x there is no cash out to pay - the round popped.
  QCOMPARE(wallet.payOut(100, 0.5), 0);
  QCOMPARE(wallet.payOut(0, 5.0), 0);
  QCOMPARE(wallet.payOut(-100, 5.0), 0);
  QCOMPARE(wallet.balance(), 1000);
}

void WalletTest::bailOutOnlyRescuesABrokePlayer()
{
  Wallet solvent(25);
  QVERIFY(!solvent.bailOut()); // can still afford the smallest bet
  QCOMPARE(solvent.balance(), 25);

  Wallet broke(24);
  QVERIFY(broke.bailOut());
  QCOMPARE(broke.balance(), 500);

  // And it cannot be farmed once the player is back on their feet.
  QVERIFY(!broke.bailOut());
}

QTEST_APPLESS_MAIN(WalletTest)

#include "WalletTest.moc"
