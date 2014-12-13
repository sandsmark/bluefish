#ifndef SNAPCHAT_H
#define SNAPCHAT_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QJsonObject>
#include <QDebug>
#include <QStringList>
#include "snapmodel.h"
#include "friendsmodel.h"
#include "snap.h"

class QHttpMultiPart;

class Snapchat : public QObject
{
    Q_OBJECT

    enum FriendStatus {
        Confirmed = 0,
        Unconfirmed = 1,
        Blocked = 2
    };
    enum Privacy {
        PrivacyEveryone = 0,
        PrivacyFriendsOnly = 1
    };
    enum UserAction {
        AddFriend,
        DeleteFriend,
        BlockUser,
        UnblockUser
    };

    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY busyChanged)

public:
    explicit Snapchat(QObject *parent = 0);

    SnapModel *snapModel() { return m_snapModel; }
    FriendsModel *friendsModel() { return m_friendsModel; }
    bool isLoggedIn() { return m_loggedIn; }

public slots:
    void login();
    void logout();
    void getUpdates(qulonglong timelimit = 0);
    void getStories(qulonglong timelimit = 0);
    void getStoryBlob(const QString &id, const QByteArray &key, const QByteArray &iv);
    void getSnap(const Snap &snap);
    void markViewed(const QString &id, int duration = 1);
    void setPrivacy(Privacy privacy);
    void changeRelationship(QString username, UserAction userAction);
    void sendSnap(const QVariantMap snapInfo);

    void networkError(QNetworkReply::NetworkError error) {
        qDebug() << error;
    }

    void setToken(QByteArray token) { if (token.isEmpty()) return; m_token = token; storeConfiguration(); }
    void setUsername(QString username);
    void setPassword(QString password);
    QString username() { return m_username; }
    QString password() { return m_password; }

    bool isBusy() { return m_busyCount > 0; }

signals:
    void loginFailed(QString message);
    void loggedIn();
    void logoutFailed(QString message);
    void loggedInChanged();

    void getSnapFailed();
    void storedStoryBlob(QString id);
    void snapStored(const Snap &snap);
    void markedViewed(QString id);

    void privacyChanged(Privacy privacy);

    void friendAdded(QString username);
    void userNotFound(QString username);
    void friendRequestSent(QString username);
    void friendDeleted(QString username);
    void userBlocked(QString username);
    void userUnblocked(QString username);

    void sendFailed(QString error);
    void snapSent();
    void snapUploaded(QString snap);

    void usernameChanged();
    void passwordChanged();

    void busyChanged();

private slots:
    void storeConfiguration();
    void incBusy() { m_busyCount++; if (m_busyCount == 1) emit busyChanged(); }
    void decBusy() { m_busyCount--; if (m_busyCount == 0) emit busyChanged(); }

private:
    static inline bool isVideo(const QByteArray &data) { return (data.length() > 2 && data[0] == 0 && data[1] == 0); }
    static inline bool isImage(const QByteArray &data) { return (data.length() > 2 && data[0] == '\xFF' && data[1] == '\xD8'); }
    static inline bool isZip(const QByteArray &data) { return (data.length() > 2 && data[0] == 'P' && data[1] == 'K'); }
    static inline bool isValid(const QByteArray &data) { return isVideo(data) || isImage(data) || isZip(data); }

    void writeFile(const QByteArray &data, QString filename);

    QNetworkReply *sendRequest(const QByteArray &endPoint,
                           QList<QPair<QString, QString> > data = QList<QPair<QString, QString> >(),
                           QNetworkAccessManager::Operation operation = QNetworkAccessManager::PostOperation,
                           const QByteArray &file = QByteArray());

    QHttpPart createPart(const QString &key, const QString &value);
    QJsonObject parseJsonObject(const QByteArray &data);

    QByteArray m_token;
    QNetworkAccessManager m_accessManager;

    QString m_username;
    QString m_password;

    SnapModel *m_snapModel;
    FriendsModel *m_friendsModel;
    bool m_loggedIn;
    QList<Snap> m_downloadQueue;
    QList<Snap> m_uploadQueue;
    int m_busyCount;
};

#endif // SNAPCHAT_H
