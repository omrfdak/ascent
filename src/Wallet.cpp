#include "Wallet.h"

#include <QtMath>

namespace {

constexpr qreal MinimumBet = 25.0;
constexpr qreal MaximumBet = 2500.0;

// Enough to get back to the table without making a loss meaningless.
constexpr qreal BailoutBalance = 500.0;

// Points are only ever worth two decimals. Snapping to them here is what keeps
// a balance equal to the number on screen instead of a hair away from it.
//
// The nudge is not cosmetic: 25.655 * 100 is 2565.4999999999995 in binary, so
// rounding it directly would give 25.65 and quietly break the halfway rule.
qreal toPoints(qreal amount)
{
  constexpr qreal HalfwayNudge = 1e-9;

  return qRound(amount * 100.0 + HalfwayNudge) / 100.0;
}

} // namespace

Wallet::Wallet(qreal startingBalance, QObject *parent)
  : QObject(parent)
  , m_balance(toPoints(startingBalance))
{
}

qreal Wallet::balance() const
{
  return m_balance;
}

qreal Wallet::minimumBet() const
{
  return MinimumBet;
}

qreal Wallet::maximumBet() const
{
  return MaximumBet;
}

bool Wallet::canPlaceBet(qreal bet) const
{
  return bet >= MinimumBet && bet <= MaximumBet && bet <= m_balance;
}

bool Wallet::placeBet(qreal bet)
{
  if (!canPlaceBet(bet))
    return false;

  m_balance = toPoints(m_balance - bet);
  emit balanceChanged();

  return true;
}

//! [pay-out]
qreal Wallet::payOut(qreal bet, qreal multiplier)
{
  if (bet <= 0.0 || multiplier < 1.0)
    return 0.0;

  // Rounded to the nearest hundredth rather than cut off: truncating would take
  // half a point from the player on an average round, which is a bigger cut than
  // the house edge the game openly charges.
  const qreal payout = toPoints(bet * multiplier);

  m_balance = toPoints(m_balance + payout);
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
