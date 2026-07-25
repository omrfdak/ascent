#ifndef CRASHCURVE_H
#define CRASHCURVE_H

#include <QObject>

/*!
  Maps the time elapsed since a round started to the multiplier shown on screen.

  The curve is exponential, so the payout grows slowly at first and then runs
  away - the reason waiting one more second stops feeling free at some point.
  It holds no state about the round itself: give it a duration, get a
  multiplier. That makes it trivial to test and safe to call from a paint loop.
*/
class CrashCurve : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qreal growthRate READ growthRate WRITE setGrowthRate NOTIFY growthRateChanged)

public:
  explicit CrashCurve(QObject *parent = nullptr);

  qreal growthRate() const;
  void setGrowthRate(qreal growthRate);

  Q_INVOKABLE qreal multiplierAt(qint64 elapsedMs) const;

signals:
  void growthRateChanged();

private:
  // Per second. At 0.1 the multiplier doubles roughly every seven seconds.
  qreal m_growthRate = 0.1;
};

#endif // CRASHCURVE_H
