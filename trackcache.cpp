#include "trackcache.h"

#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

#define db_path          "track.db"
#define db_create_string "CREATE TABLE Tracks(id integer primary key, trackid text, data text, inserttime int);"

TrackCache* TrackCache::m_instance = Q_NULLPTR;

TrackCache* TrackCache::instrance()
{
    if (!m_instance) {
        m_instance = new TrackCache;
    }
    return m_instance;
}

TrackCache::TrackCache()
    : m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    // Check if the database exists
    // If not, create it!
    QFile f(db_path);
    bool create = false;
    if (!f.exists()) {
        create = true;
    }

    m_db.setDatabaseName(db_path);
    m_db.open();
    if (create) {
        QSqlQuery qIns;
        qIns.prepare(db_create_string);
        if (!qIns.exec()) {
            qDebug() << Q_FUNC_INFO << "Can't create database";
        }
    }
}

QByteArray TrackCache::trackInfo(const QByteArray trackId)
{
    QSqlQuery q;
    q.prepare("SELECT data FROM Tracks WHERE trackid=(:trackid)");
    q.bindValue(":trackid", trackId);
    if (q.exec()) {
        if (q.first()) {
            return q.value(0).toByteArray();
        }
    } else {
        qDebug() << Q_FUNC_INFO << "DB error" << m_db.lastError().text();
    }
    return QByteArray();
}

void TrackCache::addTrack(const QByteArray trackId, const QByteArray trackInfo)
{
    QSqlQuery q;
    q.prepare("INSERT INTO Tracks (trackid, data) VALUES (:trackId, :data)");
    q.bindValue(":trackid", trackId);
    q.bindValue(":data", trackInfo);

    if (!q.exec()) {
        qDebug() << Q_FUNC_INFO << q.lastError().text();
    }
}
