#include "snap.h"

#include <QStandardPaths>
#include <QDir>
#include <QVariantMap>

Snap::Snap(const QVariantMap &map)
{
    m_id = map["id"].toString();
    m_sender = map["sn"].toString();
    m_recipient = map["rp"].toString();
    m_timeout = map["t"].toDouble();
    m_status = (Snap::SnapStatus)map["st"].toDouble();
    m_screenshots = map["c"].toDouble();
    m_type = (Snap::MediaType)map["m"].toDouble();
    m_sentAt = QDateTime::fromMSecsSinceEpoch(map["sts"].toDouble());
    m_openedAt = QDateTime::fromMSecsSinceEpoch(map["ts"].toDouble());
}

QString Snap::path() const
{
    QString path(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    path += "/snaps/";
    path += m_sender;
    path += "_";
    path += m_id;

    switch(m_type) {
    case Snap::FriendRequestVideo:
    case Snap::FriendRequestVideoNoAudio:
    case Snap::Video:
    case Snap::VideoNoAudio:
        return path + ".mp4";
    case Snap::FriendRequestImage:
    case Snap::Image:
        return path + ".jpg";
    default:
        return path + ".bin";
    }
}
