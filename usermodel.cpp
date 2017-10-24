#include "usermodel.h"
#include "tokenrepository.h"

#include <QDebug>

UserModel::UserModel()
    : QAbstractTableModel()
    , m_tokenRepository(Q_NULLPTR)
{
    connect(&m_api, &SpotifyApi::userDataReceived,
            this, &UserModel::userData);
}

UserModel::~UserModel()
{
}

void UserModel::newTokenGranted(QOAuth2AuthorizationCodeFlow *token)
{
    refreshedTokenGranted(token, QString());
}

void UserModel::refreshedTokenGranted(QOAuth2AuthorizationCodeFlow *token,
                                         QString username)
{
    bool found = false;
    foreach (UserData *data, m_users) {
        if (data->username == username) {
            found = true;
            break;
        }
    }

    if (!found) {
        UserData *ud = new UserData();
        int length = m_users.length();
        ud->posistion = length;
        ud->username = username;
        ud->token = token;
        // TODO: Add emit on rowaboutto insterted etc.

        beginInsertRows(QModelIndex(), length, length);
        m_users.append(ud);
        endInsertRows();
    }

    m_api.userData(token);
}

void UserModel::userData(QOAuth2AuthorizationCodeFlow* token,
                            QString username, QString displayName, QUrl avatar)
{
    UserData *udMatch = Q_NULLPTR;
    foreach (UserData *ud, m_users) {
        if (ud->token == token) {
            udMatch = ud;
            break;
        }
    }
    if (udMatch) {
        bool update = false;
        if (m_tokenRepository && udMatch->username.isEmpty()) {
            m_tokenRepository->addToken(username, token->token(), token->refreshToken());
        }

        if (username != udMatch->username) {
            udMatch->username = username;
            update = true;
        }
        if (displayName != udMatch->displayName) {
            udMatch->displayName = displayName;
            update = true;
        }
        if (avatar != udMatch->avatar) {
            udMatch->avatar = avatar;
            update = true;
        }

        if (update) {
            QModelIndex idx = index(udMatch->posistion, 0);
            emit dataChanged(idx, idx);
        }
    }
}

int UserModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_users.count();
}

int UserModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant UserModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < m_users.count()) {
        UserData* user = m_users[index.row()];
        if (!user)
            return QVariant();

        switch (role) {
        case UsernameRole:
            return user->username;
        case DisplayNameRole:
            return user->displayName;
        case AvatarUrlRole:
            return user->avatar;
        case TokenRole:
            return qVariantFromValue((QObject*)m_users[index.row()]->token);
        }
    }
    return QVariant();
}

QHash<int, QByteArray> UserModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UsernameRole] = "userName";
    roles[DisplayNameRole] = "displayName";
    roles[AvatarUrlRole] = "avatar";
    roles[TokenRole] = "tokenPointer";
    return roles;
}

QUrl UserModel::tokenUrl() const
{
    return m_tokenUrl;
}

QObject *UserModel::tokenRepository() const
{
    return m_tokenRepository;
}

void UserModel::setTokenUrl(QUrl tokenUrl)
{
    if (m_tokenUrl == tokenUrl)
        return;

    m_tokenUrl = tokenUrl;
    emit tokenUrlChanged(m_tokenUrl);
}

void UserModel::setTokenRepository(QObject *tokenRepository)
{
    TokenRepository *tr = qobject_cast<TokenRepository*>(tokenRepository);
    if (m_tokenRepository == tr)
        return;

    connect(tr, &TokenRepository::newTokenGranted,
            this, &UserModel::newTokenGranted);
    connect(tr, &TokenRepository::refreshedTokenGranted,
            this, &UserModel::refreshedTokenGranted);

    m_tokenRepository = tr;
    emit tokenRepositoryChanged(m_tokenRepository);
}
