#include <QApplication>
#include <FelgoApplication>

#include <QQmlApplicationEngine>

#include "CrashCurve.h"
#include "ProvablyFair.h"
#include "RoundEngine.h"
#include "Wallet.h"

namespace {

//! [register-core-types]
void registerCoreTypes()
{
    const char *uri = "Ascent";

    qmlRegisterType<CrashCurve>(uri, 1, 0, "CrashCurve");
    qmlRegisterType<ProvablyFair>(uri, 1, 0, "ProvablyFair");
    qmlRegisterType<Wallet>(uri, 1, 0, "Wallet");

    // The engine needs a curve and a wallet to mean anything, so QML gets to use
    // its states and read its properties, but not to conjure one out of nothing.
    qmlRegisterUncreatableType<RoundEngine>(uri, 1, 0, "RoundEngine",
                                            QStringLiteral("RoundEngine is created by the game, "
                                                           "not by QML"));
}
//! [register-core-types]

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FelgoApplication felgo;

    // Felgo's own font, so the game looks the same on macOS and iOS.
    felgo.setPreservePlatformFonts(false);

    registerCoreTypes();

    QQmlApplicationEngine engine;
    felgo.initialize(&engine);

    felgo.setLicenseKey(PRODUCT_LICENSE_KEY);

    // Development entry point. For publishing this becomes "qrc:/qml/AscentMain.qml",
    // which loads the QML from the binary instead of the deployed files.
    felgo.setMainQmlFileName(QStringLiteral("qml/AscentMain.qml"));

    engine.load(QUrl(felgo.mainQmlFileName()));

    return app.exec();
}
