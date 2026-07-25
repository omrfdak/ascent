#include <QtTest>

#include "RoundHistory.h"
#include "RoundSource.h"

namespace {

/*!
  A round source driven by hand, so a test can play twenty rounds without
  waiting for twenty rounds.
*/
class FakeRoundSource : public RoundSource
{
public:
  void start() override {}
  void stop() override {}
  void requestBet(qreal) override {}
  void requestCashOut() override {}

  void playRound(const QString &commitment, qreal crashPoint, const QString &seed)
  {
    emit bettingOpened(commitment, 5000);
    emit roundStarted();
    emit roundCrashed(crashPoint, seed);
  }
};

} // namespace

class RoundHistoryTest : public QObject
{
  Q_OBJECT

private slots:
  void init();

  void newestRoundComesFirst();
  void theStripKeepsItsLengthAndDropsTheOldest();
  void eachRoundKeepsWhatIsNeededToCheckIt();

private:
  std::unique_ptr<FakeRoundSource> m_rounds;
  std::unique_ptr<RoundHistory> m_history;
};

void RoundHistoryTest::init()
{
  m_rounds = std::make_unique<FakeRoundSource>();
  m_history = std::make_unique<RoundHistory>(m_rounds.get());
}

void RoundHistoryTest::newestRoundComesFirst()
{
  m_rounds->playRound(QStringLiteral("aa"), 1.50, QStringLiteral("seed-a"));
  m_rounds->playRound(QStringLiteral("bb"), 7.25, QStringLiteral("seed-b"));

  QCOMPARE(m_history->rounds().size(), 2);
  QCOMPARE(m_history->rounds().first().toMap().value(QStringLiteral("multiplier")).toReal(), 7.25);
  QCOMPARE(m_history->rounds().last().toMap().value(QStringLiteral("multiplier")).toReal(), 1.50);
}

void RoundHistoryTest::theStripKeepsItsLengthAndDropsTheOldest()
{
  const int capacity = RoundHistory::capacity();

  // Three more rounds than the strip holds, each with its own multiplier, so
  // which ones survived can be told apart.
  for (int round = 1; round <= capacity + 3; ++round)
    m_rounds->playRound(QStringLiteral("hash"), round, QStringLiteral("seed"));

  QCOMPARE(m_history->rounds().size(), capacity);

  // The last round played is at the front and the three oldest are gone.
  const QVariantList rounds = m_history->rounds();
  QCOMPARE(rounds.first().toMap().value(QStringLiteral("multiplier")).toReal(), capacity + 3);
  QCOMPARE(rounds.last().toMap().value(QStringLiteral("multiplier")).toReal(), 4.0);
}

void RoundHistoryTest::eachRoundKeepsWhatIsNeededToCheckIt()
{
  m_rounds->playRound(QStringLiteral("commitment-a"), 3.14, QStringLiteral("seed-a"));

  const QVariantMap round = m_history->rounds().first().toMap();

  // The hash that was published before the round and the seed revealed after it
  // have to end up on the same entry, or the strip records a result nobody can
  // verify.
  QCOMPARE(round.value(QStringLiteral("commitment")).toString(), QStringLiteral("commitment-a"));
  QCOMPARE(round.value(QStringLiteral("seed")).toString(), QStringLiteral("seed-a"));
  QCOMPARE(round.value(QStringLiteral("multiplier")).toReal(), 3.14);
}

QTEST_APPLESS_MAIN(RoundHistoryTest)

#include "RoundHistoryTest.moc"
