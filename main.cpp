#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <qtwebengineglobal.h>

#include "usermodel.h"
#include "trackmodel.h"
#include "tokenrepository.h"
#include "clientidentifier.h"

static QString cId;
static QString cSec;

static QObject *clientIdProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    ClientIdentifier *clientId = new ClientIdentifier(cId, cSec);
    return clientId;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("Spotify Live Wall");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addPositionalArgument("clientid", "The Spotify Client ID");
    parser.addPositionalArgument("clientsecret", "The Spotify Client Secret");

    parser.process(app);
    if (parser.positionalArguments().length()>=1)
        cId = parser.positionalArguments().at(0);
    if (parser.positionalArguments().length()>=2)
        cSec = parser.positionalArguments().at(1);

    if (cId.isEmpty() || cSec.isEmpty()) {
        qWarning() << "No Client Id or Client Secret in start arguments.";
        return -1;
    }

    // Register QML Type that are used in QML later
    qmlRegisterType<UserModel>("com.combitech.livewall", 1, 0, "UserModel");
    qmlRegisterType<TrackModel>("com.combitech.livewall", 1, 0, "TrackModel");
    qmlRegisterType<TokenRepository>("com.combitech.livewall", 1, 0, "TokenRepository");
    qmlRegisterSingletonType<ClientIdentifier>("com.combitech.livewall", 1, 0, "ClientIdentifier", clientIdProvider);
            QtWebEngine::initialize();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
