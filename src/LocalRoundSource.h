#ifndef LOCALROUNDSOURCE_H
#define LOCALROUNDSOURCE_H

#include "RoundSource.h"

// Both appear in properties QML reads, and a property type has to be complete.
#include "CrashCurve.h"
#include "RoundEngine.h"

#include <QElapsedTimer>
#include <QTimer>

class ProvablyFair;
class Wallet;

/*!
  Runs the rounds on the device.

  It generates a seed, publishes its hash, waits out the betting window, then
  drives the engine off a timer until the round pops and starts the next one.
  The seed is drawn before betting opens and revealed after the crash, so the
  player can check every round they played.

  The window lengths are properties rather than constants so that a test can run
  a hundred rounds in the time the game takes to run one.
*/
class LocalRoundSource : public RoundSource
{
  Q_OBJECT
  Q_PROPERTY(RoundEngine *engine READ engine CONSTANT)
  Q_PROPERTY(CrashCurve *curve READ curve CONSTANT)
  Q_PROPERTY(QString commitment READ commitment NOTIFY commitmentChanged)
  Q_PROPERTY(int bettingMsRemaining READ bettingMsRemaining NOTIFY bettingMsRemainingChanged)
  Q_PROPERTY(int bettingWindowMs READ bettingWindowMs WRITE setBettingWindowMs
             NOTIFY bettingWindowMsChanged)
  Q_PROPERTY(int resultWindowMs READ resultWindowMs WRITE setResultWindowMs
             NOTIFY resultWindowMsChanged)

public:
  explicit LocalRoundSource(Wallet *wallet, QObject *parent = nullptr);

  RoundEngine *engine() const;
  CrashCurve *curve() const;

  QString commitment() const;

  int bettingMsRemaining() const;

  int bettingWindowMs() const;
  void setBettingWindowMs(int bettingWindowMs);

  int resultWindowMs() const;
  void setResultWindowMs(int resultWindowMs);

  void start() override;
  void stop() override;

  void requestBet(qreal amount) override;
  void requestCashOut() override;

signals:
  void commitmentChanged();
  void bettingMsRemainingChanged();
  void bettingWindowMsChanged();
  void resultWindowMsChanged();

private:
  void openBetting();
  void beginRound();
  void tick();
  void setBettingMsRemaining(int bettingMsRemaining);

  CrashCurve *m_curve;
  ProvablyFair *m_fair;
  RoundEngine *m_engine;

  QTimer m_clockTimer;
  QTimer m_windowTimer;
  QElapsedTimer m_phaseClock;

  QString m_seed;
  QString m_commitment;

  int m_bettingMsRemaining = 0;
  int m_bettingWindowMs = 5000;
  int m_resultWindowMs = 3000;
};

#endif // LOCALROUNDSOURCE_H
