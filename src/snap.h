#ifndef SNAP_H
#define SNAP_H

#include <QFile>
#include <QDateTime>
#include <QString>

struct Snap {
public:
    enum SnapStatus {
        NoStatus = -1,
        Sent,
        Delivered,
        Opened,
        Screenshotted
    };

    enum MediaType {
        Image = 0,
        Video,
        VideoNoAudio,
        FriendRequestImage,
        FriendRequestVideo,
        FriendRequestVideoNoAudio
    };

    Snap(const QVariantMap &map);

    void setRecipient(QString recipient) { m_recipient = recipient; }
    void setSender(QString sender) { m_sender = sender; }
    void setTimeout(int timeout) { m_timeout = timeout; }
    void setStatus(SnapStatus status) { m_status = status; }
    void setScreenshots(int screenshots) { m_screenshots = screenshots; }
    void setType(MediaType type) { m_type = type; }
    void setSentAt(QDateTime sentAt) { m_sentAt = sentAt; }
    void setOpenedAt(QDateTime openedAt) { m_openedAt = openedAt; }

    QString id() const { return m_id; }
    QString recipient() const { return m_recipient; }
    QString sender() const { return m_sender; }
    int timeout() const { return m_timeout; }
    SnapStatus status() const { return m_status; }
    int screenshots() const { return m_screenshots; }
    MediaType type() const { return m_type; }
    QDateTime sentAt() const { return m_sentAt; }
    QDateTime openedAt() const { return m_openedAt; }
    bool downloaded() const { return QFile::exists(path()); }
    QString path() const;

    bool operator ==(const Snap &other) const { return m_id == other.m_id; }

private:
    QString m_id;
    QString m_recipient;
    QString m_sender;
    int m_timeout;
    SnapStatus m_status;
    int m_screenshots;
    MediaType m_type;
    QDateTime m_sentAt;
    QDateTime m_openedAt;
};

#endif // SNAP_H
