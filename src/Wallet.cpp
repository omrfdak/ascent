#include "Wallet.h"

#include <QtMath>

namespace {

constexpr qint64 MinimumBet = 25;
constexpr qint64 MaximumBet = 2500;

// Enough to get back to the table without making a loss meaningless.
constexpr qint64 BailoutBalance = 500;

} // namespace

Wallet::Wallet(qint64 startingBalance, QObject *parent)
  : QObject(parent)
  , m_balance(startingBalance)
{
}

qint64 Wallet::balance() const
{
  return m_balance;
}

qint64 Wallet::minimumBet() const
{
  return MinimumBet;
}

qint64 Wallet::maximumBet() const
{
  return MaximumBet;
}

bool Wallet::canPlaceBet(qint64 bet) const
{
  return bet >= MinimumBet && bet <= MaximumBet && bet <= m_balance;
}

bool Wallet::placeBet(qint64 bet)
{
  if (!canPlaceBet(bet))
    return false;

  m_balance -= bet;
  emit balanceChanged();

  return true;
}

//! [pay-out]
qint64 Wallet::payOut(qint64 bet, qreal multiplier)
{
  if (bet <= 0 || multiplier < 1.0)
    return 0;

  // The multiplier is shown with two decimals, so two decimals is all it is
  // worth. Turning it into hundredths here means the payout is decided by
  // integer arithmetic - the same bet at the same multiplier always pays the
  // same number of points, on every platform.
  const qint64 hundredths = qRound64(multiplier * 100.0);
  const qint64 payout = bet * hundredths / 100;

  m_balance += payout;
  emit balanceChanged();

  return payout;
}
//! [pay-out]

bool Wallet::bailOut()
{
  // Only for a player who is actually stuck, otherwise it is free money.
  if (m_balance >= MinimumBet)
    return false;

  m_balance = BailoutBalance;
  emit balanceChanged();

  return true;
}
