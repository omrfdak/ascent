#include "LocalRoundSource.h"

#include "CrashCurve.h"
#include "ProvablyFair.h"
#include "RoundEngine.h"

namespace {

// Roughly a frame. The multiplier is read off the clock rather than counted up,
// so a missed tick shows as a small jump, never as a round that runs long.
constexpr int ClockIntervalMs = 16;

} // namespace

LocalRoundSource::LocalRoundSource(Wallet *wallet, QObject *parent)
  : RoundSource(parent)
  , m_curve(new CrashCurve(this))
  , m_fair(new ProvablyFair(this))
  , m_engine(new RoundEngine(m_curve, wallet, this))
{
  m_clockTimer.setInterval(ClockIntervalMs);
  m_windowTimer.setSingleShot(true);

  connect(&m_clockTimer, &QTimer::timeout, this, &LocalRoundSource::tick);

  connect(m_engine, &RoundEngine::cashedOut, this,
          [this](qint64 payout, qreal multiplier) { emit cashOutConfirmed(payout, multiplier); });
}

RoundEngine *LocalRoundSource::engine() const
{
  return m_engine;
}

CrashCurve *LocalRoundSource::curve() const
{
  return m_curve;
}

QString LocalRoundSource::commitment() const
{
  return m_commitment;
}

int LocalRoundSource::bettingWindowMs() const
{
  return m_bettingWindowMs;
}

void LocalRoundSource::setBettingWindowMs(int bettingWindowMs)
{
  if (m_bettingWindowMs == bettingWindowMs)
    return;

  m_bettingWindowMs = bettingWindowMs;
  emit bettingWindowMsChanged();
}

int LocalRoundSource::resultWindowMs() const
{
  return m_resultWindowMs;
}

void LocalRoundSource::setResultWindowMs(int resultWindowMs)
{
  if (m_resultWindowMs == resultWindowMs)
    return;

  m_resultWindowMs = resultWindowMs;
  emit resultWindowMsChanged();
}

void LocalRoundSource::start()
{
  if (m_windowTimer.isActive() || m_clockTimer.isActive())
    return;

  openBetting();
}

void LocalRoundSource::stop()
{
  m_clockTimer.stop();
  m_windowTimer.stop();
}

void LocalRoundSource::requestBet(qint64 amount)
{
  if (m_engine->placeBet(amount))
    emit betAccepted(amount);
  else
    emit betRejected(amount);
}

void LocalRoundSource::requestCashOut()
{
  // A confirmed cash out is announced by the engine, which is what the game
  // listens to. Only the refusal is answered here.
  if (!m_engine->cashOut())
    emit cashOutRejected();
}

//! [open-betting]
void LocalRoundSource::openBetting()
{
  // The seed is drawn, and its hash published, before anyone can bet on the
  // round it decides. That order is the whole promise.
  m_seed = m_fair->generateSeed();
  m_commitment = m_fair->commitmentFor(m_seed);
  emit commitmentChanged();

  emit bettingOpened(m_commitment, m_bettingWindowMs);

  m_windowTimer.disconnect();
  connect(&m_windowTimer, &QTimer::timeout, this, &LocalRoundSource::beginRound);
  m_windowTimer.start(m_bettingWindowMs);
}
//! [open-betting]

void LocalRoundSource::beginRound()
{
  if (!m_engine->startRound(m_fair->crashPointFor(m_seed)))
    return;

  m_roundClock.start();
  m_clockTimer.start();

  emit roundStarted();
}

void LocalRoundSource::tick()
{
  m_engine->advanceTo(m_roundClock.elapsed());

  if (m_engine->state() != RoundEngine::Crashed)
    return;

  m_clockTimer.stop();

  // The seed comes out with the crash, never before it.
  emit roundCrashed(m_engine->crashPoint(), m_seed);

  m_engine->settle();

  m_windowTimer.disconnect();
  connect(&m_windowTimer, &QTimer::timeout, this, [this] {
    m_engine->openBetting();
    openBetting();
  });
  m_windowTimer.start(m_resultWindowMs);
}
