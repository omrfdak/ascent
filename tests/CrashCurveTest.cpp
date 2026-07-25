#include <QtTest>

#include "CrashCurve.h"

class CrashCurveTest : public QObject
{
  Q_OBJECT

private slots:
  void startsAtExactlyOne();
  void clampsTimeBeforeTheStart();
  void growsMonotonically();
  void matchesKnownPoints();
  void growthRateChangesTheSpeed();
};

void CrashCurveTest::startsAtExactlyOne()
{
  const CrashCurve curve;

  // Not "close to one": the first frame of a round shows 1.00x, and a player
  // cashing out instantly has to get exactly their bet back.
  QCOMPARE(curve.multiplierAt(0), 1.0);
}

void CrashCurveTest::clampsTimeBeforeTheStart()
{
  const CrashCurve curve;

  // Clocks can hand us a negative delta around a round boundary.
  QCOMPARE(curve.multiplierAt(-1), 1.0);
  QCOMPARE(curve.multiplierAt(-5000), 1.0);
}

void CrashCurveTest::growsMonotonically()
{
  const CrashCurve curve;

  qreal previous = curve.multiplierAt(0);
  for (qint64 elapsedMs = 50; elapsedMs <= 60000; elapsedMs += 50) {
    const qreal current = curve.multiplierAt(elapsedMs);
    QVERIFY2(current > previous,
             qPrintable(QStringLiteral("multiplier did not grow at %1 ms").arg(elapsedMs)));
    previous = current;
  }
}

void CrashCurveTest::matchesKnownPoints()
{
  const CrashCurve curve;

  // At the default rate the multiplier doubles every ln(2)/0.1 seconds and
  // reaches 10x after ln(10)/0.1. Both are hand-checkable numbers, which is
  // what makes them useful as a regression net.
  QVERIFY(qAbs(curve.multiplierAt(6931) - 2.0) < 0.001);
  QVERIFY(qAbs(curve.multiplierAt(23026) - 10.0) < 0.001);
}

void CrashCurveTest::growthRateChangesTheSpeed()
{
  CrashCurve slow;
  slow.setGrowthRate(0.05);

  CrashCurve fast;
  fast.setGrowthRate(0.2);

  QSignalSpy spy(&fast, &CrashCurve::growthRateChanged);
  fast.setGrowthRate(0.2);
  QCOMPARE(spy.count(), 0); // setting the same rate again is not a change

  QVERIFY(fast.multiplierAt(5000) > slow.multiplierAt(5000));
  // The rate only stretches the curve, it never moves the starting point.
  QCOMPARE(slow.multiplierAt(0), fast.multiplierAt(0));
}

QTEST_APPLESS_MAIN(CrashCurveTest)

#include "CrashCurveTest.moc"
