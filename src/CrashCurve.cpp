#include "CrashCurve.h"

#include <QtMath>

CrashCurve::CrashCurve(QObject *parent)
  : QObject(parent)
{
}

qreal CrashCurve::growthRate() const
{
  return m_growthRate;
}

void CrashCurve::setGrowthRate(qreal growthRate)
{
  if (qFuzzyCompare(m_growthRate, growthRate))
    return;

  m_growthRate = growthRate;
  emit growthRateChanged();
}

//! [multiplier-at]
qreal CrashCurve::multiplierAt(qint64 elapsedMs) const
{
  // A round can only pay out from 1.00x upwards, so time before the start is
  // not negative winnings - it is simply the start.
  if (elapsedMs <= 0)
    return 1.0;

  return qExp(m_growthRate * (elapsedMs / 1000.0));
}
//! [multiplier-at]
