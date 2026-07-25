#include "RoundHistory.h"

#include <QVariantMap>

#include "RoundSource.h"

namespace {

// Ten fits across a phone screen and covers about a minute of play, which is
// as far back as a decision about the next bet ever reaches.
constexpr int Capacity = 10;

} // namespace

RoundHistory::RoundHistory(RoundSource *source, QObject *parent)
  : QObject(parent)
{
  connect(source, &RoundSource::bettingOpened, this,
          [this](const QString &commitment, int) { onBettingOpened(commitment); });
  connect(source, &RoundSource::roundCrashed, this, &RoundHistory::onRoundCrashed);
}

QVariantList RoundHistory::rounds() const
{
  return m_rounds;
}

int RoundHistory::capacity()
{
  return Capacity;
}

void RoundHistory::onBettingOpened(const QString &commitment)
{
  m_commitment = commitment;
}

//! [record-round]
void RoundHistory::onRoundCrashed(qreal crashPoint, const QString &revealedSeed)
{
  // Newest first: the strip is read from the end the player's eye starts at.
  m_rounds.prepend(QVariantMap{ { QStringLiteral("multiplier"), crashPoint },
                                { QStringLiteral("commitment"), m_commitment },
                                { QStringLiteral("seed"), revealedSeed } });

  while (m_rounds.size() > Capacity)
    m_rounds.removeLast();

  emit roundsChanged();
}
//! [record-round]
