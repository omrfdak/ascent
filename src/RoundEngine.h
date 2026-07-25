#ifndef ROUNDENGINE_H
#define ROUNDENGINE_H

#include <QObject>

class CrashCurve;
class Wallet;

/*!
  Runs one round at a time and decides what the player is allowed to do in it.

  The engine never invents a crash point. It is told where the round pops before
  it starts, which is what keeps the promise made by the published commitment -
  nothing that happens during the round, least of all the size of the bet, can
  move the outcome.

  It has no clock of its own either. Someone else advances it, so a test can run
  a full round in microseconds and the game can run the same code off a timer.
*/
class RoundEngine : public QObject
{
  Q_OBJECT
  Q_PROPERTY(State state READ state NOTIFY stateChanged)
  Q_PROPERTY(qreal multiplier READ multiplier NOTIFY multiplierChanged)
  Q_PROPERTY(qint64 bet READ bet NOTIFY betChanged)
  Q_PROPERTY(bool hasCashedOut READ hasCashedOut NOTIFY hasCashedOutChanged)

public:
  enum State {
    Betting,  // taking bets, the balloon is on the ground
    Running,  // the multiplier is climbing
    Crashed,  // it popped, losing bets are gone
    Settled   // results are on screen, waiting to open the next round
  };
  Q_ENUM(State)

  explicit RoundEngine(CrashCurve *curve, Wallet *wallet, QObject *parent = nullptr);

  State state() const;
  qreal multiplier() const;
  qint64 bet() const;
  qreal crashPoint() const;
  bool hasCashedOut() const;

  // Puts the player's bet in for the round that is about to start.
  Q_INVOKABLE bool placeBet(qint64 bet);

  // Starts the round at the crash point decided before any bet was seen.
  Q_INVOKABLE bool startRound(qreal crashPoint);

  // Moves the round to a point in time and pops it if it got that far.
  Q_INVOKABLE bool advanceTo(qint64 elapsedMs);

  // Takes the money at the multiplier currently on screen.
  Q_INVOKABLE bool cashOut();

  // Closes the round and opens betting for the next one.
  Q_INVOKABLE bool settle();
  Q_INVOKABLE bool openBetting();

signals:
  void stateChanged();
  void multiplierChanged();
  void betChanged();
  void hasCashedOutChanged();

  void roundStarted();
  void cashedOut(qint64 payout, qreal multiplier);
  void crashed(qreal crashPoint);

private:
  void setState(State state);
  void setMultiplier(qreal multiplier);
  void setHasCashedOut(bool hasCashedOut);

  CrashCurve *m_curve;
  Wallet *m_wallet;

  State m_state = Betting;
  qreal m_multiplier = 1.0;
  qreal m_crashPoint = 1.0;
  qint64 m_bet = 0;
  bool m_hasCashedOut = false;
};

#endif // ROUNDENGINE_H
