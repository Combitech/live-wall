#include "spotifyapi.h"
#include "tokenrepository.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QOAuth2AuthorizationCodeFlow>

#include "trackcache.h"

SpotifyApi::SpotifyApi(QObject *parent) : QObject(parent)
{
}

SpotifyApi::~SpotifyApi()
{
}

bool SpotifyApi::userData(QOAuth2AuthorizationCodeFlow *token)
{
    QUrl url("https://api.spotify.com/v1/me");
    QNetworkReply *reply = token->get(url);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << Q_FUNC_INFO << reply->errorString() << reply->error();
        } else {
            const auto data = reply->readAll();
            const auto document = QJsonDocument::fromJson(data);
            const auto root = document.object();
            const auto images = root.value("images").toArray();

            QString displayName = root.value("display_name").toString();
            if (displayName.isEmpty()) {
                displayName = root.value("id").toString();
            }

            emit userDataReceived(token,
                                  root.value("id").toString(),
                                  displayName,
                                  QUrl(images.first().toObject().value("url").toString()));
        }
        reply->deleteLater();
    });
    return true;
}

bool SpotifyApi::recentlyPlayed(QOAuth2AuthorizationCodeFlow *token)
{
    QUrl url("https://api.spotify.com/v1/me/player/recently-played");
    QNetworkReply *reply = token->get(url);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << Q_FUNC_INFO << reply->errorString() << reply->error();
        } else {
            const auto data = reply->readAll();
            const auto document = QJsonDocument::fromJson(data);
            const auto root = document.object();
            const auto items = root.value("items").toArray();
            for (auto it = items.constBegin(); it!=items.constEnd(); ++it) {
                auto item = *it;
                auto track = item.toObject().value("track").toObject();

                auto artists = track.value("artists").toArray();
                QStringList artistList;
                for (auto artistIt = artists.constBegin();
                     artistIt!=artists.constEnd();
                     ++artistIt)
                {
                    auto artist = *artistIt;
                    artistList << artist.toObject().value("name").toString();
                }
                emit trackReceived(track.value("id").toString().toLatin1(),
                                   track.value("name").toString(),
                                   artistList,
                                   QDateTime::fromString(item.toObject().value("played_at").toString(),
                                                                                     "yyyy-MM-ddTHH:mm:ss.zzzZ"),
                                   false);
            }
            emit allTrackReceived();
        }
        reply->deleteLater();

    });
    return true;
}

bool SpotifyApi::trackImageUrl(QOAuth2AuthorizationCodeFlow *token, QByteArray trackId)
{
    // Chech if we have this in cache already
    QByteArray cacheData = TrackCache::instrance()->trackInfo(trackId);

    if (cacheData.isEmpty()) {
        QString urlStr = "https://api.spotify.com/v1/tracks/" + QString::fromLatin1(trackId);
        QUrl url(urlStr);
        QNetworkReply *reply = token->get(url);

        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() != QNetworkReply::NoError) {
                qDebug() << Q_FUNC_INFO << reply->errorString() << reply->error();
            } else {
                const auto data = reply->readAll();

                //Insert into cache
                TrackCache::instrance()->addTrack(trackId, data);

                parseTrackInfo(trackId, data);
            }
            reply->deleteLater();
        });
    } else {
        parseTrackInfo(trackId, cacheData);
    }
    return true;
}

void SpotifyApi::parseTrackInfo(const QByteArray trackId, const QByteArray data)
{
    const auto document = QJsonDocument::fromJson(data);
    const auto root = document.object();
    const auto album = root.value("album").toObject();
    const auto images = album.value("images").toArray();
    for (auto it = images.constBegin(); it!=images.constEnd(); ++it) {
        auto img = *it;
        int height = img.toObject().value("height").toInt();
        if (height > 250 && height < 350) {
            emit trackImageUrlReceived(trackId,
                                       QUrl(img.toObject().value("url").toString()));
        }
    }
}

bool SpotifyApi::liveTrack(QOAuth2AuthorizationCodeFlow *token)
{
    QUrl url("https://api.spotify.com/v1/me/player/currently-playing");
    QNetworkReply *reply = token->get(url);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << Q_FUNC_INFO << reply->errorString() << reply->error();
        } else {
            qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            const auto data = reply->readAll();
            const auto document = QJsonDocument::fromJson(data);
            const auto root = document.object();
            const auto item = root.value("item").toObject();
            auto artists = item.value("artists").toArray();
            QStringList artistList;
            for (auto artistIt = artists.constBegin();
                 artistIt!=artists.constEnd();
                 ++artistIt)
            {
                auto artist = *artistIt;
                artistList << artist.toObject().value("name").toString();
            }
            qDebug() << item << root.value("timestamp");
            qDebug() << item.value("name").toString() << artistList;
        }
        reply->deleteLater();

    });
    return true;
}
