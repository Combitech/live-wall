#include "tokenrepository.h"
//#include "clientid.h"

#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QNetworkRequest>
#include <QDebug>

#define TOKEN_FILE "token-repo.txt"

quint16 TokenRepository::m_nextPort = 8888;

TokenRepository::TokenRepository(QQuickItem *parent)
    : QQuickItem(parent)
{
}

void TokenRepository::componentComplete()
{
    readTokensFromFile();
}

bool TokenRepository::addToken(const QString username, const QString tokenStr, const QString refreshToken)
{
    //qDebug() << Q_FUNC_INFO << refreshToken;
    bool added = false;
    bool changed = false;
    if (!m_tokens.contains(username)) {
        added = true;
        changed = true;
    }

    if (m_tokens[username].first != tokenStr || m_tokens[username].second != refreshToken)
        changed = true;

    m_tokens[username].first = tokenStr;
    m_tokens[username].second = refreshToken;

    if (changed)
        writeTokensToFile();

    return added;
}

QList<QString> TokenRepository::users() const
{
    return m_tokens.keys();
}

QOAuth2AuthorizationCodeFlow* TokenRepository::token(const QString username,
                                                     const QString accessToken,
                                                     const QString refreshToken) const
{
    QOAuth2AuthorizationCodeFlow *token = new QOAuth2AuthorizationCodeFlow();
    auto replyHandler = new QOAuthHttpServerReplyHandler(m_nextPort++);
    replyHandler->setCallbackPath("cb");
    token->setReplyHandler(replyHandler);
    token->setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    token->setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    token->setClientIdentifier(m_clientId);
    token->setClientIdentifierSharedKey(m_clientSecret);
    token->setScope("user-read-private user-read-recently-played user-read-currently-playing user-read-playback-state");
    token->setToken(accessToken);
    token->setRefreshToken(refreshToken);
    token->setProperty("username", username);
    token->setModifyRequestFunction([=](QAbstractOAuth::Stage stage, QNetworkRequest *request) {
        qDebug() << Q_FUNC_INFO << (int)stage;
        if (request && stage == QAbstractOAuth::Stage::RefreshingAccessToken) {
            QByteArray b = m_clientId.toLatin1() + ":" +
                           m_clientSecret.toLatin1();
            QByteArray auth = "Basic " + b.toBase64();
            qDebug() << auth;
            request->setRawHeader("Authorization", auth);
        }
    });
    connect(token, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this, &TokenRepository::openUrl);
    connect(token, &QOAuth2AuthorizationCodeFlow::granted,
            this, &TokenRepository::tokenGranted);

    return token;
}

void TokenRepository::tokenGranted()
{
    if (sender()->property("username").isValid()) {
        // If the property username is valid this is a "refresed" token
        // Make sure we write it to file

        QOAuth2AuthorizationCodeFlow* token = qobject_cast<QOAuth2AuthorizationCodeFlow*>(sender());
        addToken(sender()->property("username").toString(),
                 token->token(),
                 token->refreshToken());

        emit refreshedTokenGranted(qobject_cast<QOAuth2AuthorizationCodeFlow*>(sender()),
                                   sender()->property("username").toString());

        if (!m_tokensToBeCreated.isEmpty()) {
            refreshNextPendingToken();
        }
    } else {
        // If this token do not have a usernamed set yet it's new
        // Emit a signal and let some gather the username info.
        emit newTokenGranted(qobject_cast<QOAuth2AuthorizationCodeFlow*>(sender()));
    }
}

void TokenRepository::newToken()
{
    QOAuth2AuthorizationCodeFlow *token = new QOAuth2AuthorizationCodeFlow();
    auto replyHandler = new QOAuthHttpServerReplyHandler(m_nextPort++);
    replyHandler->setCallbackPath("cb");
    token->setReplyHandler(replyHandler);
    token->setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    token->setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    token->setClientIdentifier(m_clientId);
    token->setClientIdentifierSharedKey(m_clientSecret);
    token->setScope("user-read-private user-read-recently-played user-read-currently-playing user-read-playback-state");

    connect(token, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
            this, &TokenRepository::openUrl);
    //connect(token, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser,
    //         &QDesktopServices::openUrl);
    connect(token, &QOAuth2AuthorizationCodeFlow::granted,
            this, &TokenRepository::tokenGranted);

    token->grant();
}

void TokenRepository::writeTokensToFile()
{
    QTemporaryFile tmpFile;
    tmpFile.setAutoRemove(false);

    if (tmpFile.open()) {
        QHash<QString, QPair<QString, QString>>::const_iterator i = m_tokens.begin();
        while (i != m_tokens.end()) {
            QTextStream out(&tmpFile);
            out << i.key() << ":"
                << i.value().first << ":"
                << i.value().second << "\n";
            //qDebug() << Q_FUNC_INFO << i.value().second;
            ++i;
        }
        i = m_tokensToBeCreated.begin();
        while (i != m_tokensToBeCreated.end()) {
            QTextStream out(&tmpFile);
            out << i.key() << ":"
                << i.value().first << ":"
                << i.value().second << "\n";
            //qDebug() << Q_FUNC_INFO << i.value().second;
            ++i;
        }
        tmpFile.close();
        if (QFile::exists(TOKEN_FILE)) {
            QFile::remove(TOKEN_FILE);
        }
        tmpFile.rename(TOKEN_FILE);
    }
}

void TokenRepository::readTokensFromFile()
{
    QFile f(TOKEN_FILE);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.startsWith("#") && line.contains(":")) {
                QStringList l = line.split(":");
                if (l.length() == 3 && !m_tokensToBeCreated.contains(l[0])) {
                    m_tokensToBeCreated[l[0]].first = l[1];
                    m_tokensToBeCreated[l[0]].second = l[2];
                }
            }
        }
    }
    if (!m_tokensToBeCreated.isEmpty()) {
        refreshNextPendingToken();
    }
}

QString TokenRepository::clientId() const
{
    return m_clientId;
}

void TokenRepository::setClientId(QString clientId)
{
    if (m_clientId == clientId)
        return;

    m_clientId = clientId;
    emit clientIdChanged(m_clientId);
}

QString TokenRepository::clientSecret() const
{
    return m_clientSecret;
}
void TokenRepository::setClientSecret(QString clientSecret)
{
    if (m_clientSecret == clientSecret)
        return;

    m_clientSecret = clientSecret;
    emit clientIdChanged(m_clientSecret);
}

void TokenRepository::refreshNextPendingToken()
{
    if (!m_tokensToBeCreated.isEmpty()) {
        //qDebug() << Q_FUNC_INFO << m_tokensToBeCreated.begin().key() << m_tokensToBeCreated.begin().value().second;
        auto t = token(m_tokensToBeCreated.begin().key(),
                       m_tokensToBeCreated.begin().value().first,
                       m_tokensToBeCreated.begin().value().second);
        m_tokensToBeCreated.remove(m_tokensToBeCreated.begin().key());
        t->refreshAccessToken();
    }
}
