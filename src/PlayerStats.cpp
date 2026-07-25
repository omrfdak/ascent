#include "PlayerStats.h"

#include "RoundSource.h"

PlayerStats::PlayerStats(RoundSource *rounds, QObject *parent)
  : QObject(parent)
{
  connect(rounds, &RoundSource::betAccepted, this, &PlayerStats::onBetAccepted);
  connect(rounds, &RoundSource::cashOutConfirmed, this, &PlayerStats::onCashOutConfirmed);
  connect(rounds, &RoundSource::roundCrashed, this, &PlayerStats::onRoundCrashed);
}

int PlayerStats::roundsPlayed() const
{
  return m_roundsPlayed;
}

int PlayerStats::roundsWon() const
{
  return m_roundsWon;
}

qreal PlayerStats::bestMultiplier() const
{
  return m_bestMultiplier;
}

int PlayerStats::currentStreak() const
{
  return m_currentStreak;
}

int PlayerStats::bestStreak() const
{
  return m_bestStreak;
}

qreal PlayerStats::totalWagered() const
{
  return m_totalWagered;
}

qreal PlayerStats::totalReturned() const
{
  return m_totalReturned;
}

//! [save-restore]
QVariantMap PlayerStats::save() const
{
  return { { QStringLiteral("roundsPlayed"), m_roundsPlayed },
           { QStringLiteral("roundsWon"), m_roundsWon },
           { QStringLiteral("bestMultiplier"), m_bestMultiplier },
           { QStringLiteral("currentStreak"), m_currentStreak },
           { QStringLiteral("bestStreak"), m_bestStreak },
           { QStringLiteral("totalWagered"), m_totalWagered },
           { QStringLiteral("totalReturned"), m_totalReturned } };
}

void PlayerStats::restore(const QVariantMap &saved)
{
  // Every key falls back to the value a new player starts with, so a save from
  // an older build that never knew about a statistic still loads.
  m_roundsPlayed = saved.value(QStringLiteral("roundsPlayed"), 0).toInt();
  m_roundsWon = saved.value(QStringLiteral("roundsWon"), 0).toInt();
  m_bestMultiplier = saved.value(QStringLiteral("bestMultiplier"), 0.0).toReal();
  m_currentStreak = saved.value(QStringLiteral("currentStreak"), 0).toInt();
  m_bestStreak = saved.value(QStringLiteral("bestStreak"), 0).toInt();
  m_totalWagered = saved.value(QStringLiteral("totalWagered"), 0.0).toReal();
  m_totalReturned = saved.value(QStringLiteral("totalReturned"), 0.0).toReal();

  emit statsChanged();
}
//! [save-restore]

void PlayerStats::onBetAccepted(qreal amount)
{
  m_stakeInPlay = amount;
  m_cashedOutThisRound = false;
  m_totalWagered += amount;

  emit statsChanged();
}

void PlayerStats::onCashOutConfirmed(qreal payout, qreal multiplier)
{
  m_cashedOutThisRound = true;
  m_totalReturned += payout;
  m_bestMultiplier = qMax(m_bestMultiplier, multiplier);

  emit statsChanged();
}

//! [settle-the-round]
void PlayerStats::onRoundCrashed()
{
  if (m_stakeInPlay <= 0.0)
    return;

  ++m_roundsPlayed;

  if (m_cashedOutThisRound) {
    ++m_roundsWon;
    ++m_currentStreak;
    m_bestStreak = qMax(m_bestStreak, m_currentStreak);
  } else {
    m_currentStreak = 0;
  }

  m_stakeInPlay = 0.0;
  m_cashedOutThisRound = false;

  emit statsChanged();
}
//! [settle-the-round]
