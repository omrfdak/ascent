#include <QtTest>

#include "PlayerStats.h"
#include "RoundSource.h"

namespace {

/*!
  A round source with nothing behind it. The statistics are meant to work off
  any source, so the test drives the signals by hand instead of waiting for a
  real round to play out.
*/
class FakeRoundSource : public RoundSource
{
public:
  void start() override {}
  void stop() override {}
  void requestBet(qreal) override {}
  void requestCashOut() override {}

  void bet(qreal amount) { emit betAccepted(amount); }
  void cashOut(qreal payout, qreal multiplier) { emit cashOutConfirmed(payout, multiplier); }
  void crash() { emit roundCrashed(2.0, QStringLiteral("seed")); }
};

} // namespace

class PlayerStatsTest : public QObject
{
  Q_OBJECT

private slots:
  void init();

  void watchedRoundsAreNotCounted();
  void aRoundIsCountedWhenItEnds();
  void streakSurvivesWinsAndBreaksOnALoss();
  void bestMultiplierKeepsTheHighest();
  void savedStatsComeBackTheSame();

private:
  std::unique_ptr<FakeRoundSource> m_rounds;
  std::unique_ptr<PlayerStats> m_stats;
};

void PlayerStatsTest::init()
{
  m_rounds = std::make_unique<FakeRoundSource>();
  m_stats = std::make_unique<PlayerStats>(m_rounds.get());
}

void PlayerStatsTest::watchedRoundsAreNotCounted()
{
  m_rounds->crash();
  m_rounds->crash();

  // Sitting through two rounds without betting is not two rounds played, and
  // it is certainly not two losses.
  QCOMPARE(m_stats->roundsPlayed(), 0);
  QCOMPARE(m_stats->currentStreak(), 0);
}

void PlayerStatsTest::aRoundIsCountedWhenItEnds()
{
  m_rounds->bet(100);

  // The bet is in play: wagered already, but the round has not happened yet.
  QCOMPARE(m_stats->totalWagered(), 100.0);
  QCOMPARE(m_stats->roundsPlayed(), 0);

  m_rounds->cashOut(250, 2.5);
  m_rounds->crash();

  QCOMPARE(m_stats->roundsPlayed(), 1);
  QCOMPARE(m_stats->roundsWon(), 1);
  QCOMPARE(m_stats->totalReturned(), 250.0);
}

void PlayerStatsTest::streakSurvivesWinsAndBreaksOnALoss()
{
  for (int round = 0; round < 3; ++round) {
    m_rounds->bet(50);
    m_rounds->cashOut(75, 1.5);
    m_rounds->crash();
  }

  QCOMPARE(m_stats->currentStreak(), 3);
  QCOMPARE(m_stats->bestStreak(), 3);

  // Bet, sat through the pop: the streak is gone but the record is not.
  m_rounds->bet(50);
  m_rounds->crash();

  QCOMPARE(m_stats->currentStreak(), 0);
  QCOMPARE(m_stats->bestStreak(), 3);
  QCOMPARE(m_stats->roundsPlayed(), 4);
  QCOMPARE(m_stats->roundsWon(), 3);
}

void PlayerStatsTest::bestMultiplierKeepsTheHighest()
{
  m_rounds->bet(25);
  m_rounds->cashOut(125, 5.0);
  m_rounds->crash();

  m_rounds->bet(25);
  m_rounds->cashOut(50, 2.0);
  m_rounds->crash();

  // The second round paid less, which does not make it a better round to have
  // been in - the record is the highest multiplier ever reached, not the last.
  QCOMPARE(m_stats->bestMultiplier(), 5.0);
}

void PlayerStatsTest::savedStatsComeBackTheSame()
{
  m_rounds->bet(200);
  m_rounds->cashOut(640, 3.2);
  m_rounds->crash();

  PlayerStats restored(m_rounds.get());
  restored.restore(m_stats->save());

  QCOMPARE(restored.roundsPlayed(), m_stats->roundsPlayed());
  QCOMPARE(restored.roundsWon(), m_stats->roundsWon());
  QCOMPARE(restored.bestMultiplier(), 3.2);
  QCOMPARE(restored.bestStreak(), 1);
  QCOMPARE(restored.totalWagered(), 200.0);
  QCOMPARE(restored.totalReturned(), 640.0);

  // Nothing saved at all is a new player rather than an error.
  PlayerStats fresh(m_rounds.get());
  fresh.restore(QVariantMap());

  QCOMPARE(fresh.roundsPlayed(), 0);
  QCOMPARE(fresh.bestMultiplier(), 0.0);
}

QTEST_APPLESS_MAIN(PlayerStatsTest)

#include "PlayerStatsTest.moc"
