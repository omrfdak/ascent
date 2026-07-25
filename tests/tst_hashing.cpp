#include <QtTest>
#include <QCryptographicHash>

// The fairness proof works by publishing the hash of a seed before a round and
// revealing the seed afterwards. That only proves anything if the hash is
// reproducible byte for byte, on every platform the game ships to. These tests
// pin that assumption down before anything is built on top of it.
class HashingTest : public QObject
{
  Q_OBJECT

private slots:
  void sha256MatchesKnownVector();
  void sha256IsDeterministic();
  void sha256ChangesWithInput();
};

void HashingTest::sha256MatchesKnownVector()
{
  // Published SHA-256 test vector for the input "abc".
  const QByteArray digest = QCryptographicHash::hash("abc", QCryptographicHash::Sha256);

  QCOMPARE(digest.toHex(),
           QByteArray("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
}

void HashingTest::sha256IsDeterministic()
{
  const QByteArray seed = "ascent-round-seed";

  const QByteArray first = QCryptographicHash::hash(seed, QCryptographicHash::Sha256);
  const QByteArray second = QCryptographicHash::hash(seed, QCryptographicHash::Sha256);

  QCOMPARE(first, second);
  // The commitment is shown to the player as hex, so its length is fixed too.
  QCOMPARE(first.toHex().size(), 64);
}

void HashingTest::sha256ChangesWithInput()
{
  // A single flipped character has to produce an unrelated digest, otherwise a
  // player could guess the seed from the commitment.
  const QByteArray a = QCryptographicHash::hash("ascent-round-1", QCryptographicHash::Sha256);
  const QByteArray b = QCryptographicHash::hash("ascent-round-2", QCryptographicHash::Sha256);

  QVERIFY(a != b);
}

QTEST_APPLESS_MAIN(HashingTest)

#include "tst_hashing.moc"
