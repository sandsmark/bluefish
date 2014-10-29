#include "snapchat.h"
#include <QHttpMultiPart>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QUuid>
#include <QDateTime>
#include <QStringList>
#include <QSettings>

#include <QDebug>

#include "friendsmodel.h"

#include "crypto.h"

#define BASE_URL QStringLiteral("https://feelinsonice-hrd.appspot.com/bq/")
//#define BASE_URL QStringLiteral("https://127.0.0.1/")


Snapchat::Snapchat(QObject *parent) :
    QObject(parent),
    m_outputPath("/home/sandsmark/tmp/"),
    m_snapModel(new SnapModel),
    m_friendsModel(new FriendsModel),
    m_loggedIn(false)
{
    QSettings settings(QStringLiteral("Bluefish"));
    m_token = settings.value(QStringLiteral("token"), DEFAULT_TOKEN).toByteArray();

    if (!settings.contains("username") || !settings.contains("password")) {
        qWarning() << "passed empty username, and nothing stored";
        emit loginFailed(tr("Passed empty username or password!"));
        return;
    }

    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();

    connect(this, SIGNAL(passwordChanged()), SLOT(storeConfiguration()));
    connect(this, SIGNAL(usernameChanged()), SLOT(storeConfiguration()));
    connect(this, SIGNAL(loggedIn()), SLOT(getUpdates()));
    connect(m_snapModel, SIGNAL(needSnapBlob(QString)), SLOT(getSnap(QString)));
    connect(this, SIGNAL(snapStored(QString)), m_snapModel, SLOT(snapDownloaded(QString)));

    if (m_token != DEFAULT_TOKEN) {
        m_loggedIn = true;
        getUpdates();
    } else {
        login();
    }
}

void Snapchat::login()
{
    if (m_username.isEmpty()) {
        qWarning() << "tried to login without username";
        return;
    }
    if (m_password.isEmpty()) {
        qWarning() << "tried to login without password";
        return;
    }

    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("password"), m_password));

    QNetworkReply *reply = sendRequest("login", data);

    qDebug() << "LOGGING IN";
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        qDebug() << "got result from login";
        QByteArray result = reply->readAll();
        QJsonObject object = parseJsonObject(result);
        if (!object.contains("auth_token")) {
            // no auth token in result "{"message":"Dette er ikke riktig passord. Beklager!","status":-100,"logged":false}"
            qDebug() << "no auth token in result" << result;
            QString errorMessage = object["message"].toString();
            if (errorMessage.isEmpty()) {
                emit loginFailed(tr("Login failed (no auth token in result)"));
            } else {
                emit loginFailed(tr("Login failed (%1)").arg(object["message"].toString()));
            }
            m_loggedIn = false;
            return;
        }
        m_token = object["auth_token"].toString().toLatin1();

        if (!object.contains("username")) {
            qDebug() << "no username in result" << result;
            emit loginFailed(tr("Login failed (no username in result)"));
            m_loggedIn = false;
            return;
        }
        m_username = object["username"].toString();
        m_loggedIn = true;
        emit loggedInChanged();
        emit loggedIn();
    });
}

void Snapchat::logout()
{
    QNetworkReply *reply = sendRequest("logout");
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray result = reply->readAll();
        if (!result.isEmpty() || reply->error() != QNetworkReply::NoError) {
            qDebug() << "logout failed:" << result;
            QJsonObject object = parseJsonObject(result);
            QString errorMessage = object["message"].toString();
            if (errorMessage.isEmpty()) {
                emit logoutFailed(tr("Logout failed"));
            } else {
                emit logoutFailed(tr("Logout failed (%1)").arg(object["message"].toString()));
            }
        } else {
            m_loggedIn = false;
            emit loggedInChanged();
        }
    });
}

void Snapchat::getUpdates(qulonglong timelimit)
{
    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("update_timestamp"), QString::number(timelimit)));

    QNetworkReply *reply = sendRequest("updates", data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray result = reply->readAll();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "error while fetching update:" << reply->errorString() << result;
            m_loggedIn = false;
            emit loggedInChanged();
        }
        QJsonObject updatesObject = parseJsonObject(result);
        QJsonArray snaps = updatesObject["snaps"].toArray();
        if (snaps.isEmpty()) {
            qWarning() << "no snaps";
        } else {
            m_snapModel->parseJson(snaps);
        }

        QJsonArray friends = updatesObject["friends"].toArray();
        if (friends.isEmpty()) {
            qWarning() << "no friends";
        } else {
            m_friendsModel->parseJson(friends);
        }
    });
}

void Snapchat::getStories(qulonglong timelimit)
{
    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("update_timestamp"), QString::number(timelimit)));

     QNetworkReply *reply = sendRequest("all_updates", data);
    /*QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray result = reply->readAll();
        QJsonObject object = parseJsonObject(result);
        if (object.contains("auth_token")) {
            m_token = object["auth_token"].toString().toLatin1();
        }

        // TODO: parse
        qDebug() << "--- got update ---\n"<< result << "\n---";
    });*/
}

void Snapchat::getStoryBlob(const QString &id, const QByteArray &key, const QByteArray &iv)
{
    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("story_id"), id));


    QNetworkReply *reply = sendRequest("story_blob", data, QNetworkAccessManager::GetOperation);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        //storeFile(Crypto::decryptStory(reply->readAll(), key, iv), "story_" + id);

        emit storedStoryBlob(id);
    });
}

void Snapchat::getSnap(QString id)
{
    qDebug() << "getting snap" << id;
    SnapModel::Snap *snap = m_snapModel->getSnap(id);
    if (!snap) {
        qWarning() << "unable to find snap" << id;
        return;
    }

    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("id"), id));

    QNetworkReply *reply = sendRequest("blob", data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "getting snap failed" << reply->errorString();
            qDebug() << reply->readAll();
            return;
        }
        QByteArray result = reply->readAll();
        QByteArray decoded = Crypto::decrypt(result);
        QByteArray data;
        if (isValid(decoded)) {
            data = decoded;
        } else if (isValid(result)) {
            data = result;
        } else {
            qDebug() << "weird result:" << result.left(10).toHex() << "id:" << id;
            data = result;
        }

        qDebug() << "writing" << data.length() / (1024.0 * 1024.0) << "MB to" << snap->path;

        Q_ASSERT(data.length() < 1024 * 1024 * 10);

        /*QString extension;
        if (isVideo(data)) {
            extension = ".mp4";
        } else if (isImage(data)) {
            extension = ".jpg";
        } else if (isZip(data)) {
            extension = ".zip";
        } else {
            extension = ".bin";
        }
        snap->path += extension;*/

        QFile file(snap->path);
        if (!file.open(QIODevice::WriteOnly)) {
            qWarning() << "unable to write file" << snap->path;
            emit writeFileFailed();
            return;
        }

        file.write(data);
        file.close();

        emit snapStored(id);
    });
}

void Snapchat::markViewed(const QString &id, int duration)
{
    QJsonArray events;

    {
        int timestamp = QDateTime::currentMSecsSinceEpoch() / 1000;

        QJsonObject paramObject;
        paramObject.insert("id", QString(id));

        QJsonObject viewedObject;
        viewedObject.insert("eventName", QStringLiteral("SNAP_VIEW"));
        viewedObject.insert("params", paramObject);
        viewedObject.insert("ts", QString::number(timestamp - duration));
        events.append(viewedObject);

        QJsonObject expiredObject;
        expiredObject.insert("eventName", QStringLiteral("SNAP_EXPIRED"));
        expiredObject.insert("params", paramObject);
        expiredObject.insert("ts", QString::number(timestamp));
        events.append(expiredObject);
    }

    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("id"), id));
    data.append(qMakePair(QStringLiteral("events"), QString::fromUtf8(QJsonDocument(events).toJson())));

    QNetworkReply *reply = sendRequest("update_snaps", data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray result = reply->readAll();
        if (result.isEmpty()) {
        //    emit markedViewed(id);
        } else {
            qWarning() << "failed to mark as viewed: " << result;
        }
    });
}

void Snapchat::setPrivacy(Snapchat::Privacy privacy)
{
    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("action"), QStringLiteral("updatePrivacy")));
    data.append(qMakePair(QStringLiteral("privacySetting"), QString::number(privacy)));

    QNetworkReply *reply = sendRequest("settings", data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        /*QJsonObject object = parseJsonObject(reply->readAll());
        if (!object.contains("param")) {
            qWarning() << "failed to call set privacy";
            return;
        }
        if (object["param"].isDouble() != privacy) {
            qWarning() << "failed to change privacy";
        }

        emit privacyChanged(privacy);*/
    });

}

void Snapchat::changeRelationship(QString username, UserAction userAction)
{
    QByteArray action;
    switch (userAction) {
    case AddFriend:
        action = "add";
        break;
    case DeleteFriend:
        action = "delete";
        break;
    case BlockUser:
        action = "block";
        break;
    case UnblockUser:
        action = "unblock";
        break;
    default:
        break;
    }

    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("action"), QStringLiteral("add")));
    data.append(qMakePair(QStringLiteral("friend"), username));

    QNetworkReply *reply = sendRequest("friend", data);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        // Returns JSON response.
        // Expected messages:
        // Success: '{username} is now your friend!'
        // Pending: '{username} is private. Friend request sent.'
        // Failure: 'Sorry! Couldn't find {username}'

      /*  QJsonObject result = parseJsonObject(reply->readAll());
        if (userAction == AddFriend) {
            qDebug() << result;
            Q_ASSERT(0);
        } else if (userAction == DeleteFriend)  {
            qDebug() << result;
            Q_ASSERT(0);
        } else if (userAction == BlockUser) {
            if (!result.contains("message")) {
                qWarning() << "unable to find message in result";
            }
            if (result["message"].toString() != username + " was blocked") {
                qWarning() << "failed to block user";
            }

            emit userBlocked(username);
        } else if (userAction == UnblockUser) {
            if (!result.contains("message")) {
                qWarning() << "unable to find message in result";
            }
            if (result["message"].toString() != username + " was unblocked") {
                qWarning() << "failed to unblock user";
            }

            emit userUnblocked(username);
        }*/
    });
}

void Snapchat::sendSnap(const QByteArray &fileData, QList<QString> recipients, int time)
{
    QString mediaId(m_username.toUpper() + "~" + QUuid::createUuid().toString());

    QList<QPair<QString, QString>> data;
    data.append(qMakePair(QStringLiteral("media_id"), mediaId));

    QNetworkReply *reply = sendRequest("upload", data, QNetworkAccessManager::PostOperation, fileData);
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        QByteArray result = reply->readAll();
        qDebug() << "result from uploading:" << result;
        //emit snapUploaded(mediaId);
    });
        /*
        QByteArray response = reply->readAll();
        if (!response.isEmpty()) {
            qDebug() << "unabled to upload: " << response;
            return;
        }
        QNetworkRequest request(BASE_URL + "send");

        QStringList recipientStrings;
        foreach(const QString &recipient, recipients) {
            recipientStrings.append(recipient);
        }

        QList<QPair<QString, QString> > data;
        data.append(qMakePair(QStringLiteral("media_id"), mediaId));
        data.append(qMakePair(QStringLiteral("recipients"), recipientStrings.join(",")));
        data.append(qMakePair(QStringLiteral("time"), QString::number(time)));
        data.append(qMakePair(QStringLiteral("zipped"), QStringLiteral("0")));

        QNetworkReply *reply = sendRequest(request, data);
        connect(reply, SIGNAL(finished()), SIGNAL(snapSent()));
        QObject::connect(reply, &QNetworkReply::finished, [=]() {
            QByteArray response = reply->readAll();
            if (!response.isEmpty()) {
                qWarning() << "failed to send uploaded snap" << response;
            }

            emit snapSent();
        });
    });*/
}

void Snapchat::storeConfiguration()
{
    QSettings settings(QStringLiteral("Bluefish"));
    settings.setValue(QStringLiteral("token"), m_token);
    settings.setValue(QStringLiteral("username"), m_username);
    settings.setValue(QStringLiteral("password"), m_password);
}

void Snapchat::sendUploadedSnap(const QString &id, const QList<QString> &recipients, int time)
{

}


QByteArray Snapchat::extension(SnapModel::MediaType type)
{
    switch(type) {
    case SnapModel::Video:
    case SnapModel::VideoNoAudio:
        return "mp4";
    case SnapModel::Image:
        return "jpg";
    }
    return "";
}

QNetworkReply *Snapchat::sendRequest(const QByteArray &endPoint, QList<QPair<QString, QString> > data, QNetworkAccessManager::Operation operation, const QByteArray &fileData)
{
    QNetworkRequest request(BASE_URL + endPoint);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Snapchat/6.1.2 (iPhone6,2; iOS 7.0.4; gzip)");

    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
    data.append(qMakePair(QStringLiteral("timestamp"), timestamp));
    QString token = QString::fromLatin1(Crypto::requestToken(m_token, timestamp.toLatin1()));
    data.append(qMakePair(QStringLiteral("req_token"), token));
    data.append(qMakePair(QStringLiteral("username"), m_username));

    QNetworkReply *reply = 0;
    if (operation == QNetworkAccessManager::PostOperation) {
        if (fileData.isEmpty()) {
            QByteArray body;
            foreach (auto item, data) {
                body.append(item.first.toUtf8().toPercentEncoding());
                body.append("=");
                body.append(item.second.toUtf8().toPercentEncoding());
                body.append("&");
            }
            if (data.length() > 0) {
                body.chop(1); // strip last &
            }

            request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
            reply = m_accessManager.post(request, body);
        } else {
            QString mimetype;
            int mediaType;
            if (isImage(fileData)) {
                mediaType = SnapModel::Image;
                mimetype = "image/jpeg";
            } else if (isVideo(fileData)) {
                mediaType = SnapModel::Video;
                mimetype = "video/mp4";
            } else {
                qWarning() << "trying to send invalid data";
                emit sendFailed();
                return 0;
            }
            data.append(qMakePair(QStringLiteral("type"), QString::number(mediaType)));

            QHttpMultiPart *multiPart = new QHttpMultiPart;
            QHttpPart filepart;
            filepart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimetype));
            filepart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"data\""));
            filepart.setBody(Crypto::encrypt(fileData));
            multiPart->append(filepart);

            foreach (auto item, data) {
                multiPart->append(createPart(item.first, item.second));
            }

            reply = m_accessManager.post(request, multiPart);
            multiPart->setParent(reply);
        }

    } else if (operation == QNetworkAccessManager::GetOperation) {
        QUrl url = request.url();

        QUrlQuery query;
        query.addQueryItem("timestamp", timestamp);
        query.addQueryItem("req_token", token);

        foreach(auto item, data) {
            query.addQueryItem(item.first, item.second);
        }

        url.setQuery(query);

        request.setUrl(url);

        reply = m_accessManager.get(request);
    } else {
        Q_ASSERT(0);
    }

    return reply;
}

QHttpPart Snapchat::createPart(const QString &key, const QString &value)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"" + key + "\""));
    part.setBody(value.toUtf8());
    return part;
}

QJsonObject Snapchat::parseJsonObject(const QByteArray &data)
{
    QJsonDocument result = QJsonDocument::fromJson(data);
    if (result.isNull()) {
        qDebug() << "unable to parse result: " << data;
        return QJsonObject();
    }

    QJsonObject object = result.object();
    if (object.isEmpty()) {
        qDebug() << "no json object in result";
    }
    return object;
}
