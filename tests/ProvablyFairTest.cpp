#include <QtTest>

#include "ProvablyFair.h"

class ProvablyFairTest : public QObject
{
  Q_OBJECT

private slots:
  void seedsAreUnpredictable();
  void sameSeedGivesSameCrashPoint();
  void commitmentMatchesTheRevealedSeed();
  void commitmentHidesTheSeed();
  void verifyAcceptsAHonestRound();
  void verifyRejectsATamperedSeed();
  void verifyRejectsAWrongCrashPoint();
  void crashPointsAreShapedLikeACrashGame();
  void houseEdgeShavesTheCurve();
};

void ProvablyFairTest::seedsAreUnpredictable()
{
  const ProvablyFair fair;

  QSet<QString> seeds;
  for (int i = 0; i < 1000; ++i)
    seeds.insert(fair.generateSeed());

  // A repeat inside a thousand draws would mean the seed is not really secret.
  QCOMPARE(seeds.size(), 1000);
  QCOMPARE(seeds.constBegin()->size(), 64); // 32 bytes as hex
}

void ProvablyFairTest::sameSeedGivesSameCrashPoint()
{
  const ProvablyFair fair;

  // The whole scheme rests on this: the seed alone decides the round, and it
  // decides it the same way every time, on every machine.
  QCOMPARE(fair.crashPointFor("ascent-round-1"), fair.crashPointFor("ascent-round-1"));
  QVERIFY(!qFuzzyCompare(fair.crashPointFor("ascent-round-1"),
                         fair.crashPointFor("ascent-round-2")));
}

void ProvablyFairTest::commitmentMatchesTheRevealedSeed()
{
  const ProvablyFair fair;

  // The commitment is a plain SHA-256 of the seed, so a player can reproduce it
  // with any hashing tool rather than having to trust the game.
  QCOMPARE(fair.commitmentFor("abc"),
           QStringLiteral("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
}

void ProvablyFairTest::commitmentHidesTheSeed()
{
  const ProvablyFair fair;

  const QString first = fair.commitmentFor("ascent-round-1");
  const QString second = fair.commitmentFor("ascent-round-2");

  // Two seeds one character apart must not produce related commitments,
  // otherwise the published hash would leak the round.
  QVERIFY(first != second);
  QCOMPARE(first.size(), 64);
}

void ProvablyFairTest::verifyAcceptsAHonestRound()
{
  const ProvablyFair fair;

  const QString seed = fair.generateSeed();
  const QString commitment = fair.commitmentFor(seed);
  const qreal crashPoint = fair.crashPointFor(seed);

  QVERIFY(fair.verify(seed, commitment, crashPoint));
}

void ProvablyFairTest::verifyRejectsATamperedSeed()
{
  const ProvablyFair fair;

  const QString seed = fair.generateSeed();
  const QString commitment = fair.commitmentFor(seed);

  // The game claims a different seed than the one it committed to.
  const QString swapped = fair.generateSeed();

  QVERIFY(!fair.verify(swapped, commitment, fair.crashPointFor(swapped)));
}

void ProvablyFairTest::verifyRejectsAWrongCrashPoint()
{
  const ProvablyFair fair;

  const QString seed = fair.generateSeed();
  const QString commitment = fair.commitmentFor(seed);

  // Honest seed, honest commitment, but the round claims it popped elsewhere.
  QVERIFY(!fair.verify(seed, commitment, fair.crashPointFor(seed) + 0.5));
}

void ProvablyFairTest::crashPointsAreShapedLikeACrashGame()
{
  const ProvablyFair fair;

  constexpr int Rounds = 100000;

  QList<qreal> crashPoints;
  crashPoints.reserve(Rounds);

  QSet<qreal> distinct;
  int instantPops = 0;

  for (int i = 0; i < Rounds; ++i) {
    const qreal crashPoint = fair.crashPointFor(QStringLiteral("seed-%1").arg(i));

    // A round paying less than the bet would be a bug no player could survive.
    QVERIFY2(crashPoint >= 1.0,
             qPrintable(QStringLiteral("seed-%1 crashed below 1.00x").arg(i)));

    crashPoints.append(crashPoint);
    distinct.insert(crashPoint);
    if (qFuzzyCompare(crashPoint, 1.0))
      ++instantPops;
  }

  std::sort(crashPoints.begin(), crashPoints.end());

  const qreal median = crashPoints.at(Rounds / 2);
  const auto share = [&crashPoints](std::function<bool(qreal)> predicate) {
    return 100.0 * std::count_if(crashPoints.cbegin(), crashPoints.cend(), predicate) / Rounds;
  };

  // The median, not the average: the tail runs to 10000x, so a handful of huge
  // rounds drag the average wherever they please. The median is what a player
  // actually experiences, and it should sit just under 2x.
  QVERIFY2(median > 1.90 && median < 2.10,
           qPrintable(QStringLiteral("median crash point was %1").arg(median)));

  // Half the rounds short, a tenth of them long. This is the shape that makes
  // cashing out at 2x feel like a coin flip.
  const qreal shortRounds = share([](qreal m) { return m < 2.0; });
  const qreal longRounds = share([](qreal m) { return m > 10.0; });

  QVERIFY2(shortRounds > 45.0 && shortRounds < 55.0,
           qPrintable(QStringLiteral("%1% of rounds ended below 2x").arg(shortRounds)));
  QVERIFY2(longRounds > 7.0 && longRounds < 13.0,
           qPrintable(QStringLiteral("%1% of rounds passed 10x").arg(longRounds)));

  // The house edge shows up as instant pops rather than as a shaved payout.
  const qreal instantShare = 100.0 * instantPops / Rounds;
  QVERIFY2(instantShare > 1.0 && instantShare < 3.0,
           qPrintable(QStringLiteral("%1% of rounds popped instantly").arg(instantShare)));

  // Guards against the failure mode every statistic above would survive: a
  // generator that returns the same handful of values over and over.
  QVERIFY2(distinct.size() > 1000,
           qPrintable(QStringLiteral("only %1 distinct crash points").arg(distinct.size())));
}

void ProvablyFairTest::houseEdgeShavesTheCurve()
{
  ProvablyFair generous;
  generous.setHouseEdge(0.0);

  ProvablyFair greedy;
  greedy.setHouseEdge(0.5);

  // The same seed has to pay less once the house takes a bigger cut - and never
  // more, which is the direction that would cost real money.
  int shaved = 0;
  for (int i = 0; i < 100; ++i) {
    const QString seed = QStringLiteral("seed-%1").arg(i);
    const qreal cheap = greedy.crashPointFor(seed);
    const qreal rich = generous.crashPointFor(seed);

    QVERIFY(cheap <= rich);
    if (cheap < rich)
      ++shaved;
  }

  // Direction alone is not enough: an edge that is quietly ignored would satisfy
  // "never more" as well. It has to actually move the curve.
  QVERIFY2(shaved > 90, qPrintable(QStringLiteral("house edge changed only %1 of 100 rounds")
                                     .arg(shaved)));
}

QTEST_APPLESS_MAIN(ProvablyFairTest)

#include "ProvablyFairTest.moc"
