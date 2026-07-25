#include <QApplication>
#include <FelgoApplication>

#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    FelgoApplication felgo;

    // Felgo's own font, so the game looks the same on macOS and iOS.
    felgo.setPreservePlatformFonts(false);

    QQmlApplicationEngine engine;
    felgo.initialize(&engine);

    felgo.setLicenseKey(PRODUCT_LICENSE_KEY);

    // Development entry point. For publishing this becomes "qrc:/qml/AscentMain.qml",
    // which loads the QML from the binary instead of the deployed files.
    felgo.setMainQmlFileName(QStringLiteral("qml/AscentMain.qml"));

    engine.load(QUrl(felgo.mainQmlFileName()));

    return app.exec();
}
