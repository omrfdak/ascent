#include "RoundEngine.h"

#include "CrashCurve.h"
#include "Wallet.h"

#include <QtMath>

RoundEngine::RoundEngine(CrashCurve *curve, Wallet *wallet, QObject *parent)
  : QObject(parent)
  , m_curve(curve)
  , m_wallet(wallet)
{
}

RoundEngine::State RoundEngine::state() const
{
  return m_state;
}

qreal RoundEngine::multiplier() const
{
  return m_multiplier;
}

qreal RoundEngine::bet() const
{
  return m_bet;
}

qreal RoundEngine::crashPoint() const
{
  return m_crashPoint;
}

bool RoundEngine::hasCashedOut() const
{
  return m_hasCashedOut;
}

bool RoundEngine::placeBet(qreal bet)
{
  // Once the balloon is off the ground the round is closed. Letting a bet in
  // here would mean betting on a multiplier the player can already see.
  if (m_state != Betting || m_bet > 0.0)
    return false;

  if (!m_wallet->placeBet(bet))
    return false;

  m_bet = bet;
  emit betChanged();

  return true;
}

bool RoundEngine::startRound(qreal crashPoint)
{
  if (m_state != Betting || crashPoint < 1.0)
    return false;

  m_crashPoint = crashPoint;
  setHasCashedOut(false);
  setMultiplier(1.0);
  setState(Running);

  emit roundStarted();

  return true;
}

//! [advance-to]
bool RoundEngine::advanceTo(qint64 elapsedMs)
{
  if (m_state != Running)
    return false;

  // Truncating happens here and nowhere else. The player is paid at the number
  // they can read on screen, so that number - not the raw curve behind it - is
  // what the round is settled with.
  const qreal raw = m_curve->multiplierAt(elapsedMs);
  const qreal shown = qFloor(raw * 100.0) / 100.0;

  if (shown >= m_crashPoint) {
    // Never overshoot into a multiplier that was never reachable this round.
    setMultiplier(m_crashPoint);
    setState(Crashed);
    emit crashed(m_crashPoint);

    return true;
  }

  setMultiplier(shown);

  return true;
}
//! [advance-to]

//! [cash-out]
bool RoundEngine::cashOut()
{
  // Only once, only while the balloon is still up, and only with money in.
  if (m_state != Running || m_hasCashedOut || m_bet <= 0.0)
    return false;

  setHasCashedOut(true);

  const qreal payout = m_wallet->payOut(m_bet, m_multiplier);
  emit cashedOut(payout, m_multiplier);

  return true;
}
//! [cash-out]

bool RoundEngine::settle()
{
  if (m_state != Crashed)
    return false;

  setState(Settled);

  return true;
}

bool RoundEngine::openBetting()
{
  if (m_state != Settled)
    return false;

  m_bet = 0.0;
  setHasCashedOut(false);
  setMultiplier(1.0);
  setState(Betting);

  emit betChanged();

  return true;
}

void RoundEngine::setState(State state)
{
  if (m_state == state)
    return;

  m_state = state;
  emit stateChanged();
}

void RoundEngine::setHasCashedOut(bool hasCashedOut)
{
  if (m_hasCashedOut == hasCashedOut)
    return;

  m_hasCashedOut = hasCashedOut;
  emit hasCashedOutChanged();
}

void RoundEngine::setMultiplier(qreal multiplier)
{
  if (qFuzzyCompare(m_multiplier, multiplier))
    return;

  m_multiplier = multiplier;
  emit multiplierChanged();
}
