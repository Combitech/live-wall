#ifndef SPOTIFYAPI_H
#define SPOTIFYAPI_H

#include <QObject>
#include <QUrl>
#include <QDateTime>

class QOAuth2AuthorizationCodeFlow;
class SpotifyApi : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyApi(QObject *parent = nullptr);
    ~SpotifyApi();

public slots:
    bool userData(QOAuth2AuthorizationCodeFlow *token);
    bool recentlyPlayed(QOAuth2AuthorizationCodeFlow* token);
    bool trackImageUrl(QOAuth2AuthorizationCodeFlow* token, QString trackId);
    bool liveTrack(QOAuth2AuthorizationCodeFlow* token);

signals:
    void userDataReceived(QOAuth2AuthorizationCodeFlow *token,
                          QString username, QString displayName, QUrl avatar);
    void trackReceived(QString trackId, QString title, QStringList artists,
                       QDateTime playedAt, bool live);
    void allTrackReceived();
    void trackImageUrlReceived(QString trackId, QUrl url);
};

#endif // SPOTIFYAPI_H
