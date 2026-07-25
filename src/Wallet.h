#ifndef WALLET_H
#define WALLET_H

#include <QObject>

/*!
  Holds the player's points and is the only place they change.

  Points carry two decimals, because a multiplier does: cashing out 25 points at
  1.13x is worth 28.25, and paying 28 instead would be a silent cut on every
  round. Every amount that enters a balance is rounded to those two decimals
  first, so a balance is always exactly the number the player was shown rather
  than a float slowly drifting away from it.
*/
class Wallet : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qreal balance READ balance NOTIFY balanceChanged)
  Q_PROPERTY(qreal minimumBet READ minimumBet CONSTANT)
  Q_PROPERTY(qreal maximumBet READ maximumBet CONSTANT)

public:
  explicit Wallet(qreal startingBalance = 1000.0, QObject *parent = nullptr);

  qreal balance() const;

  qreal minimumBet() const;
  qreal maximumBet() const;

  // Whether this bet is within the limits and actually covered by the balance.
  Q_INVOKABLE bool canPlaceBet(qreal bet) const;

  // Takes the bet out of the balance. The points are gone until a cash out.
  Q_INVOKABLE bool placeBet(qreal bet);

  // Pays a cashed out bet back at the multiplier and returns what was credited.
  Q_INVOKABLE qreal payOut(qreal bet, qreal multiplier);

  // Refills a player who can no longer afford the smallest bet.
  Q_INVOKABLE bool bailOut();

signals:
  void balanceChanged();

private:
  qreal m_balance;
};

#endif // WALLET_H
