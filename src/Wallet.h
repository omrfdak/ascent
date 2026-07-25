#ifndef WALLET_H
#define WALLET_H

#include <QObject>

/*!
  Holds the player's points and is the only place they change.

  Everything here is whole points, never a floating point balance. A multiplier
  like 2.37x is turned into an integer number of hundredths before it touches a
  balance, so a thousand rounds of betting and cashing out land on exactly the
  number arithmetic says they should - no cent drifting away per round.
*/
class Wallet : public QObject
{
  Q_OBJECT
  Q_PROPERTY(qint64 balance READ balance NOTIFY balanceChanged)
  Q_PROPERTY(qint64 minimumBet READ minimumBet CONSTANT)
  Q_PROPERTY(qint64 maximumBet READ maximumBet CONSTANT)

public:
  explicit Wallet(qint64 startingBalance = 1000, QObject *parent = nullptr);

  qint64 balance() const;

  qint64 minimumBet() const;
  qint64 maximumBet() const;

  // Whether this bet is within the limits and actually covered by the balance.
  Q_INVOKABLE bool canPlaceBet(qint64 bet) const;

  // Takes the bet out of the balance. The points are gone until a cash out.
  Q_INVOKABLE bool placeBet(qint64 bet);

  // Pays a cashed out bet back at the multiplier and returns what was credited.
  Q_INVOKABLE qint64 payOut(qint64 bet, qreal multiplier);

  // Refills a player who can no longer afford the smallest bet.
  Q_INVOKABLE bool bailOut();

signals:
  void balanceChanged();

private:
  qint64 m_balance;
};

#endif // WALLET_H
