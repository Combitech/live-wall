#include "trackmodel.h"

#include <QDebug>

TrackModel::TrackModel()
    : QAbstractTableModel()
    , m_token(Q_NULLPTR)
{
    m_refreshTimer.setInterval(3 * 60 * 1000);
    connect(&m_refreshTimer, &QTimer::timeout,
            this, &TrackModel::fetchTracks);
    connect(&m_api, &SpotifyApi::trackReceived,
            this, &TrackModel::trackReceived);
    connect(&m_api, &SpotifyApi::allTrackReceived,
            this, &TrackModel::allTrackReceived);
    connect(&m_api, &SpotifyApi::trackImageUrlReceived,
            this, &TrackModel::trackImageUrlReceived);
}

TrackModel::~TrackModel() {
    qDeleteAll(m_tracks);
    m_tracks.clear();
}

QObject* TrackModel::token() const
{
    return m_token;
}

QString TrackModel::username() const
{
    return m_username;
}

void TrackModel::setToken(QObject *token)
{
    QOAuth2AuthorizationCodeFlow* t = qobject_cast<QOAuth2AuthorizationCodeFlow*>(token);
    if (m_token == t)
        return;

    m_token = t;
    emit tokenChanged(m_token);
    fetchTracks();
}

void TrackModel::setUsername(QString username)
{
    if (m_username == username)
        return;

    m_username = username;
    emit usernameChanged(m_username);
    fetchTracks();
}

void TrackModel::fetchTracks()
{
    if (m_username.isEmpty() || m_token == Q_NULLPTR) {
        m_refreshTimer.stop();
        return;
    }

    if (!m_refreshTimer.isActive())
        m_refreshTimer.start();

    // Mark all tracks as not updated
    foreach(Track *t, m_tracks) {
        t->updated = false;
    }

    m_api.recentlyPlayed(m_token);
    //m_api.liveTrack(m_token);
}

void TrackModel::trackReceived(QString trackId, QString title, QStringList artists,
                               QDateTime playedAt, bool live)
{
    if (!m_tracks.contains(trackId)) {
        Track* t = new Track;
        t->id = trackId;
        t->title = title;
        t->artists = artists;
        t->playedAt = playedAt;
        t->live = live;
        t->updated = true;

        // Make the list sTorted by playedAt
        int pos = 0;
        foreach (QString tId, m_trackIdList) {
            Track* tInList = m_tracks[tId];
            if (tInList) {
                if (tInList->playedAt < playedAt) {
                    break;
                }
            }
            pos++;
        }

        beginInsertRows(QModelIndex(), pos, pos);
        m_trackIdList.insert(pos, trackId);
        m_tracks[trackId] = t;
        endInsertRows();

        m_api.trackImageUrl(m_token, trackId);
    } else {
        Track *t = m_tracks[trackId];
        if (t)
            t->updated = true;
    }
}

void TrackModel::allTrackReceived()
{
    for (auto it = m_tracks.begin(); it!=m_tracks.end();) {
        Track *t = it.value();
        if (t && !t->updated) {
            int pos = 0;
            foreach(QString trackId, m_trackIdList) {
                if (trackId == t->id) {
                    break;
                }
                pos++;
            }
            beginRemoveRows(QModelIndex(), pos, pos);
            m_trackIdList.removeAt(pos);
            delete t;
            it = m_tracks.erase(it);
            endRemoveRows();
        } else {
            ++it;
        }
    }
}

void TrackModel::trackImageUrlReceived(QString trackId, QUrl url)
{
    if (m_tracks.contains(trackId)) {
        Track *t = m_tracks[trackId];
        if (t) {
            if (t->coverImage != url) {
                t->coverImage = url;

                int pos = m_trackIdList.indexOf(trackId);
                emit trackCoverAdded(pos);
                emit dataChanged(index(pos, 0), index(pos, 0));
            }
        }
    }
}

int TrackModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_trackIdList.count();
}

int TrackModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TrackModel::data(const QModelIndex &index, int role) const
{
    if (index.row() >= 0 &&
        m_trackIdList.count()>index.row() &&
        m_tracks.contains(m_trackIdList[index.row()]))
    {
        Track *track = m_tracks[m_trackIdList.at(index.row())];
        if (track) {
            switch(role) {
            case IdRole:
                return track->id;
            case TitleRole:
                return track->title;
            case ArtistsRole:
                // TODO! Fix
                return track->artists.first();
            case CoverRole:
                return track->coverImage;
            case LiveRole:
                return track->live;
            }
        }
    }
    return QVariant();
}

QHash<int, QByteArray> TrackModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "trackId";
    roles[TitleRole] = "trackTitle";
    roles[ArtistsRole] = "artists";
    roles[CoverRole] = "trackCover";
    roles[LiveRole] = "live";
    return roles;
}
