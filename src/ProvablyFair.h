#ifndef PROVABLYFAIR_H
#define PROVABLYFAIR_H

#include <QObject>
#include <QString>

/*!
  Decides where a round pops, in a way the player can check afterwards.

  Before a round starts the game generates a secret seed and publishes only its
  SHA-256 hash - the commitment. The crash point is derived from that same seed,
  so it is already fixed while the player is still deciding how much to bet.
  When the round ends the seed is revealed: anyone can hash it, compare it to the
  commitment they were shown, and recompute the crash point themselves.

  Neither side can cheat. The game cannot pick a friendlier seed afterwards
  without breaking the hash, and the player cannot read the seed out of the
  commitment.
*/
class ProvablyFair : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qreal houseEdge READ houseEdge WRITE setHouseEdge NOTIFY houseEdgeChanged)

public:
  explicit ProvablyFair(QObject *parent = nullptr);

  qreal houseEdge() const;
  void setHouseEdge(qreal houseEdge);

  //! A fresh secret seed for one round, as hex.
  Q_INVOKABLE QString generateSeed() const;

  //! What the player is shown before the round: the hash of the seed.
  Q_INVOKABLE QString commitmentFor(const QString &seed) const;

  //! Where the round pops, in multiplier units, truncated to two decimals.
  Q_INVOKABLE qreal crashPointFor(const QString &seed) const;

  //! Recomputes both halves of the proof from the revealed seed.
  Q_INVOKABLE bool verify(const QString &seed, const QString &commitment, qreal crashPoint) const;

signals:
  void houseEdgeChanged();

private:
  // The cut the house takes, expressed as a fraction of the payout.
  qreal m_houseEdge = 0.01;
};

#endif // PROVABLYFAIR_H
