#ifndef SPOTIFYMODEL_H
#define SPOTIFYMODEL_H

#include "userdata.h"
#include "spotifyapi.h"
#include "tokenrepository.h"

#include <QAbstractTableModel>
#include <QOAuth2AuthorizationCodeFlow>

class UserModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QUrl tokenUrl READ tokenUrl WRITE setTokenUrl NOTIFY tokenUrlChanged)
    Q_PROPERTY(QObject* tokenRepository READ tokenRepository WRITE setTokenRepository NOTIFY tokenRepositoryChanged)

public:
    enum SpotifyUserRoles {
        UsernameRole = Qt::UserRole + 1,
        DisplayNameRole,
        AvatarUrlRole,
        TokenRole
    };

    UserModel();
    ~UserModel();

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;    

    QUrl tokenUrl() const;    
    QObject* tokenRepository() const;

public slots:
    void setTokenUrl(QUrl tokenUrl);   
    void setTokenRepository(QObject* tokenRepository);

signals:
    void tokenUrlChanged(QUrl tokenUrl);

    void tokenRepositoryChanged(QObject* tokenRepository);

private slots:
    void newTokenGranted(QOAuth2AuthorizationCodeFlow *token);
    void refreshedTokenGranted(QOAuth2AuthorizationCodeFlow *token,
                               QString username);
    void userData(QOAuth2AuthorizationCodeFlow *token,
                  QString username, QString displayName, QUrl avatar);

private:
    QList<UserData*> m_users;
    SpotifyApi m_api;
    QUrl m_tokenUrl;
    TokenRepository *m_tokenRepository;
};

#endif // SPOTIFYMODEL_H
