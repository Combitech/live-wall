#include "clientidentifier.h"
#include <QCoreApplication>

#include <QtDebug>

ClientIdentifier::ClientIdentifier(QString id, QString secret)
    : QObject()
    , m_clientId(id)
    , m_clientSecret(secret)
{
    qDebug() << Q_FUNC_INFO << m_clientId << m_clientSecret;
}

QString ClientIdentifier::clientId() const
{
    return m_clientId;
}

QString ClientIdentifier::clientSecret() const
{
    return m_clientSecret;
}
