#include "ProvablyFair.h"

#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QtMath>

namespace {

// 52 bits is what a double can hold without losing a single integer step, so
// the fraction below is exact rather than merely close.
constexpr quint64 RandomBits = 52;
constexpr quint64 RandomRange = Q_UINT64_C(1) << RandomBits;

// Far beyond any round a player will sit through, but it keeps a seed that
// lands absurdly close to the top of the range from producing infinity.
constexpr qreal MaxCrashPoint = 10000.0;

} // namespace

ProvablyFair::ProvablyFair(QObject *parent)
  : QObject(parent)
{
}

qreal ProvablyFair::houseEdge() const
{
  return m_houseEdge;
}

void ProvablyFair::setHouseEdge(qreal houseEdge)
{
  if (qFuzzyCompare(m_houseEdge, houseEdge))
    return;

  m_houseEdge = houseEdge;
  emit houseEdgeChanged();
}

QString ProvablyFair::generateSeed() const
{
  // Not QRandomGenerator::global(): the seed is the only secret in the scheme,
  // so it comes from the system entropy source.
  QByteArray seed(32, Qt::Uninitialized);
  QRandomGenerator::system()->fillRange(reinterpret_cast<quint32 *>(seed.data()),
                                        seed.size() / sizeof(quint32));

  return QString::fromLatin1(seed.toHex());
}

//! [commitment]
QString ProvablyFair::commitmentFor(const QString &seed) const
{
  const QByteArray digest = QCryptographicHash::hash(seed.toUtf8(), QCryptographicHash::Sha256);

  return QString::fromLatin1(digest.toHex());
}
//! [commitment]

//! [crash-point]
qreal ProvablyFair::crashPointFor(const QString &seed) const
{
  const QByteArray digest = QCryptographicHash::hash(seed.toUtf8(), QCryptographicHash::Sha256);

  // Read 52 bits off the front of the digest and read them as a number in [0, 1).
  quint64 bits = 0;
  for (int i = 0; i < 7; ++i)
    bits = (bits << 8) | static_cast<quint8>(digest.at(i));
  bits &= RandomRange - 1;

  const qreal uniform = static_cast<qreal>(bits) / static_cast<qreal>(RandomRange);

  // 1/(1-x) turns a flat number into the long tail a crash game needs: half the
  // rounds end below 2x, a few run to 50x. The house edge shaves the whole curve
  // down slightly, which is also what makes an instant pop at 1.00x possible.
  const qreal rawCrashPoint = (1.0 - m_houseEdge) / (1.0 - uniform);

  // The multiplier is shown with two decimals, so it has to be decided with two
  // decimals too - a payout must never be worth more than what the player read.
  const qreal truncated = qFloor(rawCrashPoint * 100.0) / 100.0;

  return qBound(1.0, truncated, MaxCrashPoint);
}
//! [crash-point]

bool ProvablyFair::verify(const QString &seed, const QString &commitment, qreal crashPoint) const
{
  return commitmentFor(seed) == commitment && qFuzzyCompare(crashPointFor(seed), crashPoint);
}
