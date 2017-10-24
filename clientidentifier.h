#ifndef CLIENTIDENTIFIER_H
#define CLIENTIDENTIFIER_H

#include <QObject>

class ClientIdentifier : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString clientId READ clientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret NOTIFY clientSecretChanged)

public:
    explicit ClientIdentifier(QString id, QString secret);

    QString clientId() const;
    QString clientSecret() const;

signals:
    void clientIdChanged();
    void clientSecretChanged();

private:
    QString m_clientId;
    QString m_clientSecret;
};

#endif // CLIENTIDENTIFIER_H
