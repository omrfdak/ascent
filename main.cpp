#include <QApplication>
#include <FelgoApplication>

#include <QQmlApplicationEngine>

#include "LocalRoundSource.h"
#include "PlayerStats.h"
#include "ProvablyFair.h"
#include "RoundEngine.h"
#include "RoundHistory.h"
#include "Wallet.h"

namespace {

//! [register-core-types]
void registerCoreTypes(Wallet *wallet, RoundSource *rounds, PlayerStats *stats,
                       RoundHistory *history, ProvablyFair *fair)
{
    const char *uri = "Ascent";

    // There is exactly one wallet and one stream of rounds in a running game, so
    // QML is handed those two rather than the means to build its own.
    qmlRegisterSingletonInstance(uri, 1, 0, "PlayerWallet", wallet);
    qmlRegisterSingletonInstance(uri, 1, 0, "Rounds", rounds);
    qmlRegisterSingletonInstance(uri, 1, 0, "PlayerStats", stats);
    qmlRegisterSingletonInstance(uri, 1, 0, "RecentRounds", history);

    // The proof is checked with the same code that made it, which is the
    // point: the player runs the game's own arithmetic against its own claim.
    qmlRegisterSingletonInstance(uri, 1, 0, "Fairness", fair);

    // Not creatable either, but the UI needs the state names to know whether it
    // is drawing a betting window, a climbing multiplier or a wreck.
    qmlRegisterUncreatableType<RoundEngine>(uri, 1, 0, "RoundEngine",
                                            QStringLiteral("RoundEngine belongs to the round "
                                                           "source, not to QML"));
}
//! [register-core-types]

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FelgoApplication felgo;

    // Felgo's own font, so the game looks the same on macOS and iOS.
    felgo.setPreservePlatformFonts(false);

    Wallet wallet;
    LocalRoundSource rounds(&wallet);
    PlayerStats stats(&rounds);
    RoundHistory history(&rounds);

    registerCoreTypes(&wallet, &rounds, &stats, &history, rounds.fair());

    QQmlApplicationEngine engine;
    felgo.initialize(&engine);

    felgo.setLicenseKey(PRODUCT_LICENSE_KEY);

    // Development entry point. For publishing this becomes "qrc:/qml/AscentMain.qml",
    // which loads the QML from the binary instead of the deployed files.
    felgo.setMainQmlFileName(QStringLiteral("qml/AscentMain.qml"));

    engine.load(QUrl(felgo.mainQmlFileName()));

    return app.exec();
}
