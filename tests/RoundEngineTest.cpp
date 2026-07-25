#include <QtTest>

#include "CrashCurve.h"
#include "RoundEngine.h"
#include "Wallet.h"

class RoundEngineTest : public QObject
{
  Q_OBJECT

private slots:
  void init();

  void startsInTheBettingState();
  void runsAFullRoundInOrder();
  void refusesABetOnceTheRoundIsRunning();
  void refusesASecondBetInTheSameRound();
  void refusesToStartARoundThatIsAlreadyRunning();
  void popsAtTheCommittedPointAndNotBeyond();
  void refusesCashOutAfterTheCrash();
  void refusesASecondCashOut();
  void refusesCashOutWithoutABet();
  void paysTheMultiplierThatWasOnScreen();
  void losesTheBetWhenThePlayerWaitsTooLong();
  void clearsTheBetWhenTheNextRoundOpens();
  void announcesWhetherThePlayerIsStillIn();
  void autoCashOutPaysExactlyAtTheTarget();
  void autoCashOutStaysOutOfRoundsThatPopFirst();
  void autoCashOutIsAStandingInstruction();

private:
  CrashCurve m_curve;
  std::unique_ptr<Wallet> m_wallet;
  std::unique_ptr<RoundEngine> m_engine;
};

void RoundEngineTest::init()
{
  m_wallet = std::make_unique<Wallet>(1000);
  m_engine = std::make_unique<RoundEngine>(&m_curve, m_wallet.get());
}

void RoundEngineTest::startsInTheBettingState()
{
  QCOMPARE(m_engine->state(), RoundEngine::Betting);
  QCOMPARE(m_engine->multiplier(), 1.0);
  QCOMPARE(m_engine->bet(), 0.0);
}

void RoundEngineTest::runsAFullRoundInOrder()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(2.0));
  QCOMPARE(m_engine->state(), RoundEngine::Running);

  QVERIFY(m_engine->advanceTo(20000)); // well past 2.00x
  QCOMPARE(m_engine->state(), RoundEngine::Crashed);

  QVERIFY(m_engine->settle());
  QCOMPARE(m_engine->state(), RoundEngine::Settled);

  QVERIFY(m_engine->openBetting());
  QCOMPARE(m_engine->state(), RoundEngine::Betting);

  // The same steps taken out of order have to be refused.
  QVERIFY(!m_engine->settle());
  QVERIFY(!m_engine->openBetting());
  QVERIFY(!m_engine->advanceTo(1000));
}

void RoundEngineTest::refusesABetOnceTheRoundIsRunning()
{
  QVERIFY(m_engine->startRound(5.0));

  // Betting here would mean betting on a multiplier already on screen.
  QVERIFY(!m_engine->placeBet(100));
  QCOMPARE(m_engine->bet(), 0.0);
  QCOMPARE(m_wallet->balance(), 1000.0);
}

void RoundEngineTest::refusesASecondBetInTheSameRound()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(!m_engine->placeBet(100));

  QCOMPARE(m_engine->bet(), 100.0);
  QCOMPARE(m_wallet->balance(), 900.0); // the refused bet never left the wallet
}

void RoundEngineTest::refusesToStartARoundThatIsAlreadyRunning()
{
  QVERIFY(m_engine->startRound(3.0));
  QVERIFY(!m_engine->startRound(50.0));

  QCOMPARE(m_engine->crashPoint(), 3.0);
}

void RoundEngineTest::popsAtTheCommittedPointAndNotBeyond()
{
  QVERIFY(m_engine->startRound(1.5));

  QVERIFY(m_engine->advanceTo(1000));
  QCOMPARE(m_engine->state(), RoundEngine::Running);
  QVERIFY(m_engine->multiplier() < 1.5);

  QVERIFY(m_engine->advanceTo(60000));
  QCOMPARE(m_engine->state(), RoundEngine::Crashed);

  // The round is worth exactly what was committed, not what the curve had
  // reached by the time we happened to look at it.
  QCOMPARE(m_engine->multiplier(), 1.5);
}

void RoundEngineTest::refusesCashOutAfterTheCrash()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(2.0));
  QVERIFY(m_engine->advanceTo(20000));

  QVERIFY(!m_engine->cashOut());
  QCOMPARE(m_wallet->balance(), 900.0); // the bet stays lost
}

void RoundEngineTest::refusesASecondCashOut()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(5.0));
  QVERIFY(m_engine->advanceTo(6931)); // 2.00x

  QVERIFY(m_engine->cashOut());
  const qreal afterFirst = m_wallet->balance();

  QVERIFY(!m_engine->cashOut());
  QCOMPARE(m_wallet->balance(), afterFirst);
}

void RoundEngineTest::refusesCashOutWithoutABet()
{
  QVERIFY(m_engine->startRound(5.0));
  QVERIFY(m_engine->advanceTo(6931));

  // Watching a round is allowed, being paid for watching is not.
  QVERIFY(!m_engine->cashOut());
  QCOMPARE(m_wallet->balance(), 1000.0);
}

void RoundEngineTest::paysTheMultiplierThatWasOnScreen()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(5.0));

  // 6931 ms is where the curve reaches 1.99999x, which reads as 1.99 - a couple
  // of milliseconds later it has genuinely earned the second decimal.
  QVERIFY(m_engine->advanceTo(6940));
  QCOMPARE(m_engine->multiplier(), 2.0);

  QSignalSpy spy(m_engine.get(), &RoundEngine::cashedOut);
  QVERIFY(m_engine->cashOut());

  QCOMPARE(spy.count(), 1);
  const qreal payout = spy.first().at(0).toReal();
  const qreal multiplier = spy.first().at(1).toReal();

  // Whatever the curve was really at, the payout follows the two decimals the
  // player could read - here 100 points at 2.00x.
  QCOMPARE(multiplier, m_engine->multiplier());
  QCOMPARE(payout, 200.0);
  QCOMPARE(m_wallet->balance(), 1100.0);
}

void RoundEngineTest::losesTheBetWhenThePlayerWaitsTooLong()
{
  QVERIFY(m_engine->placeBet(250));
  QVERIFY(m_engine->startRound(3.0));

  QSignalSpy spy(m_engine.get(), &RoundEngine::crashed);
  QVERIFY(m_engine->advanceTo(60000));

  QCOMPARE(spy.count(), 1);
  QCOMPARE(spy.first().at(0).toReal(), 3.0);
  QCOMPARE(m_wallet->balance(), 750.0);
}

void RoundEngineTest::clearsTheBetWhenTheNextRoundOpens()
{
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(2.0));
  QVERIFY(m_engine->advanceTo(20000));
  QVERIFY(m_engine->settle());
  QVERIFY(m_engine->openBetting());

  QCOMPARE(m_engine->bet(), 0.0);
  QCOMPARE(m_engine->multiplier(), 1.0);

  // And the next round is a clean slate, not a continuation of the last one.
  QVERIFY(m_engine->placeBet(100));
}

void RoundEngineTest::announcesWhetherThePlayerIsStillIn()
{
  QSignalSpy spy(m_engine.get(), &RoundEngine::hasCashedOutChanged);

  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(5.0));
  QVERIFY(m_engine->advanceTo(6940));

  QVERIFY(!m_engine->hasCashedOut());
  QVERIFY(m_engine->cashOut());

  // The button that offers the cash out has to hear about it, otherwise it
  // keeps offering a move the engine will refuse.
  QVERIFY(m_engine->hasCashedOut());
  QCOMPARE(spy.count(), 1);

  QVERIFY(m_engine->advanceTo(60000));
  QVERIFY(m_engine->settle());
  QVERIFY(m_engine->openBetting());

  QVERIFY(!m_engine->hasCashedOut());
  QCOMPARE(spy.count(), 2);
}

void RoundEngineTest::autoCashOutPaysExactlyAtTheTarget()
{
  m_engine->setAutoCashOutAt(2.0);
  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(5.0));

  QSignalSpy cashedOut(m_engine.get(), &RoundEngine::cashedOut);

  // A clock never lands on the threshold. Advancing past it by a whole second
  // is the worst case a 16 ms tick can produce, and the payout still has to be
  // the target rather than wherever the tick happened to fall.
  QVERIFY(m_engine->advanceTo(8000));

  QCOMPARE(cashedOut.count(), 1);
  QCOMPARE(cashedOut.first().at(1).toReal(), 2.0);
  QCOMPARE(cashedOut.first().at(0).toReal(), 200.0);
  QCOMPARE(m_wallet->balance(), 1100.0);

  // The round is not over: it keeps climbing to the point it committed to, the
  // player is simply out of it.
  QCOMPARE(m_engine->state(), RoundEngine::Running);
  QVERIFY(m_engine->hasCashedOut());
}

void RoundEngineTest::autoCashOutStaysOutOfRoundsThatPopFirst()
{
  m_engine->setAutoCashOutAt(3.0);
  QVERIFY(m_engine->placeBet(100));

  // The round pops at exactly the target. A player tapping at that moment would
  // have been too late - the multiplier reads 2.99 and then it is over - so the
  // setting does not get a better deal than a hand on the button.
  QVERIFY(m_engine->startRound(3.0));
  QVERIFY(m_engine->advanceTo(60000));

  QCOMPARE(m_engine->state(), RoundEngine::Crashed);
  QVERIFY(!m_engine->hasCashedOut());
  QCOMPARE(m_wallet->balance(), 900.0);
}

void RoundEngineTest::autoCashOutIsAStandingInstruction()
{
  m_engine->setAutoCashOutAt(1.5);

  // Two rounds without touching the setting again: it is a preference, not a
  // request that belongs to one round.
  for (int round = 0; round < 2; ++round) {
    QVERIFY(m_engine->placeBet(100));
    QVERIFY(m_engine->startRound(4.0));
    QVERIFY(m_engine->advanceTo(60000));
    QVERIFY(m_engine->settle());
    QVERIFY(m_engine->openBetting());
  }

  QCOMPARE(m_engine->autoCashOutAt(), 1.5);
  QCOMPARE(m_wallet->balance(), 1100.0);

  // Off, and a value that pays back exactly the stake counts as off.
  m_engine->setAutoCashOutAt(0);
  QCOMPARE(m_engine->autoCashOutAt(), 0.0);

  m_engine->setAutoCashOutAt(1.0);
  QCOMPARE(m_engine->autoCashOutAt(), 0.0);

  QVERIFY(m_engine->placeBet(100));
  QVERIFY(m_engine->startRound(4.0));
  QVERIFY(m_engine->advanceTo(60000));

  QVERIFY(!m_engine->hasCashedOut());
}

QTEST_APPLESS_MAIN(RoundEngineTest)

#include "RoundEngineTest.moc"
