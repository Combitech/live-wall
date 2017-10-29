#ifndef TACKCACHE_H
#define TACKCACHE_H

#include <QObject>
#include <QSqlDatabase>

class TrackCache
{
public:
    static TrackCache *instrance();

    QByteArray trackInfo(const QByteArray trackId);
    void addTrack(const QByteArray trackId, const QByteArray trackInfo);


private:
    TrackCache();

    QSqlDatabase m_db;

    static TrackCache *m_instance;
};

#endif // TACKCACHE_H
