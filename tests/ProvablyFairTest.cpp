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
  void crashPointNeverPaysLessThanTheBet();
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

void ProvablyFairTest::crashPointNeverPaysLessThanTheBet()
{
  const ProvablyFair fair;

  for (int i = 0; i < 5000; ++i) {
    const qreal crashPoint = fair.crashPointFor(QStringLiteral("seed-%1").arg(i));
    QVERIFY2(crashPoint >= 1.0,
             qPrintable(QStringLiteral("seed-%1 crashed below 1.00x").arg(i)));
  }
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
