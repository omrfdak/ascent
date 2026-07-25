#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <QObject>
#include <QVariantMap>

class RoundSource;

/*!
  What the player has done so far, counted from the same signals the screen
  listens to.

  It watches a RoundSource rather than being told by the game screen, so the
  numbers are the same whether a round was played by tapping, by an automatic
  cash out, or by a server the game has not been written against yet.

  Rounds the player only watched are not counted. A round that was not bet on
  costs nothing and proves nothing, and counting it would make every statistic
  a measure of how long the game was left open.
*/
class PlayerStats : public QObject
{
  Q_OBJECT

  // One signal for the lot: every value below moves at one of two moments, and
  // both of them change several at once.
  Q_PROPERTY(int roundsPlayed READ roundsPlayed NOTIFY statsChanged)
  Q_PROPERTY(int roundsWon READ roundsWon NOTIFY statsChanged)
  Q_PROPERTY(qreal bestMultiplier READ bestMultiplier NOTIFY statsChanged)
  Q_PROPERTY(int currentStreak READ currentStreak NOTIFY statsChanged)
  Q_PROPERTY(int bestStreak READ bestStreak NOTIFY statsChanged)
  Q_PROPERTY(qreal totalWagered READ totalWagered NOTIFY statsChanged)
  Q_PROPERTY(qreal totalReturned READ totalReturned NOTIFY statsChanged)

public:
  explicit PlayerStats(RoundSource *rounds, QObject *parent = nullptr);

  int roundsPlayed() const;
  int roundsWon() const;
  qreal bestMultiplier() const;
  int currentStreak() const;
  int bestStreak() const;
  qreal totalWagered() const;
  qreal totalReturned() const;

  // The whole history as one value, so storing it is a single key.
  Q_INVOKABLE QVariantMap save() const;
  Q_INVOKABLE void restore(const QVariantMap &saved);

signals:
  void statsChanged();

private:
  void onBetAccepted(qreal amount);
  void onCashOutConfirmed(qreal payout, qreal multiplier);
  void onRoundCrashed();

  int m_roundsPlayed = 0;
  int m_roundsWon = 0;
  qreal m_bestMultiplier = 0.0;
  int m_currentStreak = 0;
  int m_bestStreak = 0;
  qreal m_totalWagered = 0.0;
  qreal m_totalReturned = 0.0;

  // What the round in progress is worth so far. A round is only counted once
  // it is over, because until then it is neither won nor lost.
  qreal m_stakeInPlay = 0.0;
  bool m_cashedOutThisRound = false;
};

#endif // PLAYERSTATS_H
