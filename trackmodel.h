#ifndef TRACKMODEL_H
#define TRACKMODEL_H

#include <QAbstractTableModel>
#include <QOAuth2AuthorizationCodeFlow>
#include <QTimer>
#include <QDateTime>
#include <QUrl>

#include "spotifyapi.h"

class TrackModel : public QAbstractTableModel
{
    class Track {
    public:
        QString id;
        QString title;
        QStringList artists;
        QUrl coverImage;
        QDateTime playedAt;
        bool live;
        bool updated;
    };

    Q_OBJECT
    Q_PROPERTY(QObject* token READ token WRITE setToken NOTIFY tokenChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)

public:
    enum SpotifyUserRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ArtistsRole,
        CoverRole,
        LiveRole
    };

    TrackModel();
    ~TrackModel();

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    QObject* token() const;
    QString username() const;

public slots:
    void setToken(QObject* token);
    void setUsername(QString username);

signals:
    void tokenChanged(QObject* token);
    void usernameChanged(QString username);
    void trackCoverAdded(int pos);

private slots:
    void fetchTracks();
    void trackReceived(QString trackId, QString title,
                       QStringList artists, QDateTime playedAt, bool live);
    void allTrackReceived();
    void trackImageUrlReceived(QString trackId, QUrl url);

private:
    QOAuth2AuthorizationCodeFlow* m_token;
    QString m_username;
    QTimer m_refreshTimer;
    QMap<QString, Track*> m_tracks;
    QList<QString> m_trackIdList;

    SpotifyApi m_api;
};

#endif // TRACKMODEL_H
