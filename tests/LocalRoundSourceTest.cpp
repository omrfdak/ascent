#include <QtTest>

#include <QCryptographicHash>

#include "CrashCurve.h"
#include "LocalRoundSource.h"
#include "ProvablyFair.h"
#include "RoundEngine.h"
#include "Wallet.h"

class LocalRoundSourceTest : public QObject
{
  Q_OBJECT

private slots:
  void init();

  void signalsArriveInTheRightOrder();
  void roundLastsAsLongAsTheCrashPointNeeds();
  void revealedSeedMatchesThePublishedCommitment();
  void betsAreAnsweredEitherWay();
  void cashOutIsConfirmedWhileRunningAndRefusedAfterTheCrash();
  void stopEndsTheCycle();

private:
  std::unique_ptr<Wallet> m_wallet;
  std::unique_ptr<LocalRoundSource> m_source;
};

void LocalRoundSourceTest::init()
{
  m_wallet = std::make_unique<Wallet>(1000);
  m_source = std::make_unique<LocalRoundSource>(m_wallet.get());

  // A round that takes seconds in the game takes milliseconds here: the windows
  // shrink and the curve is steepened, without touching the logic under test.
  m_source->setBettingWindowMs(20);
  m_source->setResultWindowMs(20);
  m_source->curve()->setGrowthRate(5.0);
}

void LocalRoundSourceTest::signalsArriveInTheRightOrder()
{
  QStringList order;

  connect(m_source.get(), &RoundSource::bettingOpened, this,
          [&order] { order.append(QStringLiteral("bettingOpened")); });
  connect(m_source.get(), &RoundSource::roundStarted, this,
          [&order] { order.append(QStringLiteral("roundStarted")); });
  connect(m_source.get(), &RoundSource::roundCrashed, this,
          [&order] { order.append(QStringLiteral("roundCrashed")); });

  QSignalSpy crashes(m_source.get(), &RoundSource::roundCrashed);
  m_source->start();

  // Two full rounds, so the cycle is shown to repeat rather than just to run.
  QVERIFY(crashes.wait(10000));
  QVERIFY(crashes.wait(10000));

  QCOMPARE(order, QStringList({ QStringLiteral("bettingOpened"),
                                QStringLiteral("roundStarted"),
                                QStringLiteral("roundCrashed"),
                                QStringLiteral("bettingOpened"),
                                QStringLiteral("roundStarted"),
                                QStringLiteral("roundCrashed") }));
}

void LocalRoundSourceTest::roundLastsAsLongAsTheCrashPointNeeds()
{
  QElapsedTimer roundClock;

  connect(m_source.get(), &RoundSource::roundStarted, this, [&roundClock] { roundClock.start(); });

  QSignalSpy crashes(m_source.get(), &RoundSource::roundCrashed);
  m_source->start();
  QVERIFY(crashes.wait(10000));

  const qint64 measuredMs = roundClock.elapsed();
  const qreal crashPoint = crashes.first().at(0).toReal();

  // The round has to last exactly as long as the curve needs to climb to the
  // point the seed committed to - not a fixed length, and not until a timer
  // somewhere decides it is over.
  const qreal expectedMs = 1000.0 * qLn(crashPoint) / m_source->curve()->growthRate();

  QVERIFY2(qAbs(measuredMs - expectedMs) < 120.0,
           qPrintable(QStringLiteral("%1x should take %2 ms, took %3 ms")
                        .arg(crashPoint)
                        .arg(expectedMs)
                        .arg(measuredMs)));
}

void LocalRoundSourceTest::revealedSeedMatchesThePublishedCommitment()
{
  QSignalSpy openings(m_source.get(), &RoundSource::bettingOpened);
  QSignalSpy crashes(m_source.get(), &RoundSource::roundCrashed);

  m_source->start();
  QVERIFY(crashes.wait(10000));

  const QString published = openings.first().at(0).toString();
  const QString revealed = crashes.first().at(1).toString();

  // What the player was shown before betting has to be the hash of what they
  // are shown afterwards. Anyone can redo this check with a hashing tool.
  const QByteArray digest = QCryptographicHash::hash(revealed.toUtf8(),
                                                     QCryptographicHash::Sha256);
  QCOMPARE(QString::fromLatin1(digest.toHex()), published);

  // And the round really was settled at the point that seed decides.
  const ProvablyFair fair;
  QCOMPARE(crashes.first().at(0).toReal(), fair.crashPointFor(revealed));
}

void LocalRoundSourceTest::betsAreAnsweredEitherWay()
{
  QSignalSpy accepted(m_source.get(), &RoundSource::betAccepted);
  QSignalSpy rejected(m_source.get(), &RoundSource::betRejected);

  m_source->requestBet(100);
  QCOMPARE(accepted.count(), 1);
  QCOMPARE(m_wallet->balance(), 900);

  // Below the minimum: refused, and answered rather than silently dropped.
  m_source->requestBet(5);
  QCOMPARE(rejected.count(), 1);
  QCOMPARE(m_wallet->balance(), 900);
}

void LocalRoundSourceTest::cashOutIsConfirmedWhileRunningAndRefusedAfterTheCrash()
{
  QSignalSpy confirmed(m_source.get(), &RoundSource::cashOutConfirmed);
  QSignalSpy refused(m_source.get(), &RoundSource::cashOutRejected);

  m_source->requestBet(100);

  QSignalSpy started(m_source.get(), &RoundSource::roundStarted);
  m_source->start();
  QVERIFY(started.wait(10000));

  m_source->requestCashOut();
  QCOMPARE(confirmed.count(), 1);
  QVERIFY(confirmed.first().at(0).toLongLong() >= 100);

  QSignalSpy crashes(m_source.get(), &RoundSource::roundCrashed);
  QVERIFY(crashes.wait(10000));

  m_source->requestCashOut();
  QCOMPARE(refused.count(), 1);
}

void LocalRoundSourceTest::stopEndsTheCycle()
{
  QSignalSpy crashes(m_source.get(), &RoundSource::roundCrashed);

  m_source->start();
  QVERIFY(crashes.wait(10000));

  m_source->stop();

  // Nothing else may start on its own once the game has stopped listening.
  QVERIFY(!crashes.wait(500));
}

QTEST_GUILESS_MAIN(LocalRoundSourceTest)

#include "LocalRoundSourceTest.moc"
