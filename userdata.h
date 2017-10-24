#ifndef USERDATA_H
#define USERDATA_H

#include <QString>
#include <QUrl>

class QOAuth2AuthorizationCodeFlow;
class UserData {
public:
    int posistion;
    QString username;
    QString displayName;
    QUrl avatar;
    QOAuth2AuthorizationCodeFlow *token;
};

#endif // USERDATA_H
