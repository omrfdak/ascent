#ifndef ROUNDSOURCE_H
#define ROUNDSOURCE_H

#include <QObject>
#include <QString>

/*!
  Where rounds come from.

  The game screen talks to this and never to the machinery behind it, so the
  same UI works whether the rounds are generated on the device or handed down
  by a server. That is also why nothing here returns a value: a bet or a cash
  out is a request that gets answered by a signal, which is what talking to a
  server looks like even when the answer happens to arrive immediately.
*/
class RoundSource : public QObject
{
  Q_OBJECT

public:
  explicit RoundSource(QObject *parent = nullptr);
  ~RoundSource() override;

  //! [round-source]
  // Starts and stops the cycle of rounds.
  Q_INVOKABLE virtual void start() = 0;
  Q_INVOKABLE virtual void stop() = 0;

  Q_INVOKABLE virtual void requestBet(qreal amount) = 0;
  Q_INVOKABLE virtual void requestCashOut() = 0;

signals:
  // The hash of the seed that decides this round, published before any bet.
  void bettingOpened(const QString &commitment, int windowMs);

  void roundStarted();

  // The seed behind the commitment, revealed now that it cannot be abused.
  void roundCrashed(qreal crashPoint, const QString &revealedSeed);

  void betAccepted(qreal amount);
  void betRejected(qreal amount);

  void cashOutConfirmed(qreal payout, qreal multiplier);
  void cashOutRejected();
  //! [round-source]
};

#endif // ROUNDSOURCE_H
