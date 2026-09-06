#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QUrl>
#include <iostream>

using namespace Qt::StringLiterals;
using namespace std;

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setOrganizationName("InnoStudio");
    app.setApplicationName("InnoStudio");
    app.setApplicationVersion(QString::fromLatin1(INNOSTUDIO_APP_VERSION));

    QQmlApplicationEngine engine;

    // 捕获所有 QML 运行期与解析期报错
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, [](const QList<QQmlError> &warnings) {
        for (const auto &w : warnings) {
            cerr << "[QML Error/Warning] " << w.toString().toStdString() << endl;
        }
    });

    const QUrl url(u"qrc:/qt/qml/InnoStudio/qml/Main.qml"_s);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl) {
                cerr << "[CRITICAL] Failed to load QML root object from: "
                     << url.toString().toStdString() << endl;
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection
    );

    engine.load(url);

    return app.exec();
}
