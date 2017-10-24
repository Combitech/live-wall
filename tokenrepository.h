#ifndef TOKENREPOSITORY_H
#define TOKENREPOSITORY_H

#include <QObject>
#include <QQuickItem>
#include <QHash>
#include <QPair>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>

class TokenRepository : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)

public:
    explicit TokenRepository(QQuickItem *parent = nullptr);
    void componentComplete();

    bool addToken(const QString username,
                  const QString tokenStr,
                  const QString refreshToken);

    QList<QString> users() const;

    Q_INVOKABLE void newToken();

    QString clientId() const;
    QString clientSecret() const;

signals:
    void newTokenGranted(QOAuth2AuthorizationCodeFlow *token);
    void refreshedTokenGranted(QOAuth2AuthorizationCodeFlow *token,
                               QString username);
    void openUrl(const QUrl url);

    void clientIdChanged(QString clientId);
    void clientSecretChanged(QString clientSecret);

public slots:
    void writeTokensToFile();
    void readTokensFromFile();

    void setClientId(QString clientId);
    void setClientSecret(QString clientSecret);

private slots:
    void tokenGranted();

private:
    QOAuth2AuthorizationCodeFlow* token(const QString username,
                                        const QString token,
                                        const QString refreshToken) const;
    void refreshNextPendingToken();

    QHash<QString, QPair<QString, QString> > m_tokens;
    QHash<QString, QPair<QString, QString> > m_tokensToBeCreated;

    QString m_clientId;
    QString m_clientSecret;

    static quint16 m_nextPort;
};

#endif // TOKENREPOSITORY_H
