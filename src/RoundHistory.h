#ifndef ROUNDHISTORY_H
#define ROUNDHISTORY_H

#include <QObject>
#include <QString>
#include <QVariantList>

class RoundSource;

/*!
  The last few rounds, newest first.

  It keeps the commitment next to the multiplier and the seed, because a round
  that cannot be checked afterwards is just a number on a strip. The list is
  short on purpose: it is there to be glanced at between bets, and a player who
  wants the whole history wants a different screen.

  Rounds are recorded whether or not they were bet on - this is what the game
  did, not what the player did.
*/
class RoundHistory : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVariantList rounds READ rounds NOTIFY roundsChanged)

public:
  explicit RoundHistory(RoundSource *source, QObject *parent = nullptr);

  QVariantList rounds() const;

  static int capacity();

signals:
  void roundsChanged();

private:
  void onBettingOpened(const QString &commitment);
  void onRoundCrashed(qreal crashPoint, const QString &revealedSeed);

  QVariantList m_rounds;

  // The hash of the round being played, held until the crash it belongs to.
  QString m_commitment;
};

#endif // ROUNDHISTORY_H
