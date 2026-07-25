#include <QApplication>
#include <FelgoApplication>

#include <QQmlApplicationEngine>

#include "LocalRoundSource.h"
#include "RoundEngine.h"
#include "Wallet.h"

namespace {

//! [register-core-types]
void registerCoreTypes(Wallet *wallet, RoundSource *rounds)
{
    const char *uri = "Ascent";

    // There is exactly one wallet and one stream of rounds in a running game, so
    // QML is handed those two rather than the means to build its own.
    qmlRegisterSingletonInstance(uri, 1, 0, "PlayerWallet", wallet);
    qmlRegisterSingletonInstance(uri, 1, 0, "Rounds", rounds);

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

    registerCoreTypes(&wallet, &rounds);

    QQmlApplicationEngine engine;
    felgo.initialize(&engine);

    felgo.setLicenseKey(PRODUCT_LICENSE_KEY);

    // Development entry point. For publishing this becomes "qrc:/qml/AscentMain.qml",
    // which loads the QML from the binary instead of the deployed files.
    felgo.setMainQmlFileName(QStringLiteral("qml/AscentMain.qml"));

    engine.load(QUrl(felgo.mainQmlFileName()));

    return app.exec();
}
