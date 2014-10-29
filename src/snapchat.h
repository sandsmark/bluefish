#ifndef SNAPCHAT_H
#define SNAPCHAT_H

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QJsonObject>
#include <QDebug>
#include "snapmodel.h"

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

    struct Snap {
        QString id;
    };

    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY loggedInChanged)

public:
    explicit Snapchat(QObject *parent = 0);

    SnapModel *snapModel() { return m_snapModel; }
    bool isLoggedIn() { return m_loggedIn; }

public slots:
    void login(QString username = QString(), QString password = QString());
    void logout();
    void getUpdates(qulonglong timelimit = 0);
    void getStories(qulonglong timelimit = 0);
    void getStoryBlob(const QString &id, const QByteArray &key, const QByteArray &iv);
    void getSnap(QString id);
    void markViewed(const QString &id, int duration = 1);
    void setPrivacy(Privacy privacy);
    void changeRelationship(QString username, UserAction userAction);
    void sendSnap(const QByteArray &fileData, QList<QString> recipients, int time = 5);

    void networkError(QNetworkReply::NetworkError error) {
        qDebug() << error;
    }

signals:
    void loginFailed(QString message);
    void loggedIn();
    void logoutFailed(QString message);
    void loggedInChanged();

    void writeFileFailed();
    void storedStoryBlob(QString id);
    void snapStored(QString id);
    void markedViewed(QString id);

    void privacyChanged(Privacy privacy);

    void friendAdded(QString username);
    void userNotFound(QString username);
    void friendRequestSent(QString username);
    void friendDeleted(QString username);
    void userBlocked(QString username);
    void userUnblocked(QString username);

    void sendFailed();
    void snapSent();
    void snapUploaded(QString snap);

private:
    static inline bool isVideo(const QByteArray &data) { return (data.length() > 2 && data[0] == 0 && data[1] == 0); }
    static inline bool isImage(const QByteArray &data) { return (data.length() > 2 && data[0] == '\xFF' && data[1] == '\xD8'); }
    static inline bool isZip(const QByteArray &data) { return (data.length() > 2 && data[0] == 'P' && data[1] == 'K'); }
    static inline bool isValid(const QByteArray &data) { return isVideo(data) || isImage(data) || isZip(data); }

    QByteArray extension(SnapModel::MediaType type);

    QNetworkReply *sendRequest(const QByteArray &endPoint,
                           QList<QPair<QString, QString> > data = QList<QPair<QString, QString> >(),
                           QNetworkAccessManager::Operation operation = QNetworkAccessManager::PostOperation,
                           const QByteArray &file = QByteArray());

    QHttpPart createPart(const QString &key, const QString &value);
    QJsonObject parseJsonObject(const QByteArray &data);
    void sendUploadedSnap(const QString &id, const QList<QString> &recipients, int time);

    QByteArray m_token;
    QNetworkAccessManager m_accessManager;
    QString m_username;

    QString m_outputPath;
    SnapModel *m_snapModel;
    bool m_loggedIn;
};

#endif // SNAPCHAT_H
