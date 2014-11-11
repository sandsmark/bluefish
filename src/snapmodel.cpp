#include <QJsonObject>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>

#include "snapmodel.h"
#include "snapchat.h"

SnapModel::SnapModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void SnapModel::snapDownloaded(QString id)
{
    qDebug() << "snap downloaded" << id;
    Snap *snap = 0;
    int snapIndex=0;
    for (snapIndex=0; snapIndex<m_snaps.size(); snapIndex++) {
        if (m_snaps[snapIndex].id == id) {
            snap = &m_snaps[snapIndex];
            break;
        }
    }
    if (!snap) {
        qWarning() << "unable to find snap" << id;
        return;
    }
    snap->downloaded = true;
    emit dataChanged(index(snapIndex), index(snapIndex));
}

void SnapModel::snapGone(QString id)
{
    int snapIndex = -1;
    for (snapIndex=0; snapIndex<m_snaps.size(); snapIndex++) {
        if (m_snaps[snapIndex].id == id) {
            break;
        }
    }

    beginRemoveRows(QModelIndex(), snapIndex, snapIndex + 1);
    m_snaps.removeAt(snapIndex);
}

SnapModel::Snap *SnapModel::getSnap(QString id)
{
    for (int i=0; i<m_snaps.size(); i++) {
        if (m_snaps[i].id == id) {
            return &m_snaps[i];
        }
    }
    return 0;
}

QString SnapModel::getFilePath(const SnapModel::Snap &snap)
{
    QString path(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    path += "/snaps/";
    path += snap.sender;
    path += "_";
    path += snap.id;
    if (snap.type == Image) {
        path += ".jpg";
    } else {
        path += ".mp4";
    }

    return path;
}

QHash<int, QByteArray> SnapModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[IdRole] = "snapId";
    roleNames[SenderRole] = "sender";
    roleNames[RecipientRole] = "recipient";
    roleNames[TimeoutRole] = "timeout";
    roleNames[StatusRole] = "status";
    roleNames[ScreenshotCountRole] = "screenshotCount";
    roleNames[MediaTypeRole] = "type";
    roleNames[SentAtRole] = "sentAt";
    roleNames[OpenedAtRole] = "openedAt";
    roleNames[PathRole] = "filePath";
    roleNames[DownloadedRole] = "downloaded";
    return roleNames;
}

QVariant SnapModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qWarning() << "model got queried for invalid index";
        return QVariant();
    }

    if (index.row() > rowCount()) {
        qWarning() << "model got queried for row out of bounds";
        return QVariant();
    }

    const Snap &snap = m_snaps[index.row()];

    switch (role) {
    case IdRole:
        return snap.id;
    case RecipientRole:
        return snap.recipient;
    case SenderRole:
        return snap.sender;
    case TimeoutRole:
        return snap.timeout;
    case StatusRole:
        return snap.status;
    case ScreenshotCountRole:
        return snap.screenshots;
    case MediaTypeRole:
        return snap.type;
    case SentAtRole: {
        QDateTime now = QDateTime::currentDateTime();
        int daysAgo = snap.sentAt.daysTo(now);
        if (daysAgo > 1) {
            return tr("%1 days ago").arg(daysAgo);
        }
        int secsAgo = snap.sentAt.secsTo(now);
        if (secsAgo > 3600) {
            return tr("%1 hours ago").arg(secsAgo / 3600);
        }
        if (secsAgo > 60) {
            return tr("%1 minutes ago").arg(secsAgo / 60);
        }
        return tr("%1 seconds ago").arg(secsAgo);
    }
    case OpenedAtRole:
        return snap.openedAt;
    case PathRole:
        return snap.path;
    case DownloadedRole:
        return snap.downloaded;
    default:
        return QVariant();
    }
}

void SnapModel::parseJson(const QJsonArray &snaps)
{
    foreach(const QJsonValue item, snaps) {
        //qDebug() << "adding snap:" << item;
        QJsonObject object = item.toObject();
        if (object.contains("c_id")) {
            qDebug() << "skipping sent snap";
            continue;
        }
        SnapStatus status = (SnapModel::SnapStatus)object["st"].toDouble();
        if (status != Delivered) {
            qDebug() << "not handling snap that is not just delivered";
            continue;
        }

        SnapModel::MediaType mediaType = (SnapModel::MediaType)object["m"].toDouble();
        if (mediaType != SnapModel::Image && mediaType != SnapModel::Video && mediaType != SnapModel::VideoNoAudio) {
            qDebug() << "unrecognized mediatype";
            continue;
        }
        Snap snap;
        snap.id = object["id"].toString();
        snap.sender = object["sn"].toString();
        snap.recipient = object["rp"].toString();
        snap.timeout = object["t"].toDouble();
        snap.status = status;
        snap.screenshots = object["c"].toDouble();
        snap.type = mediaType;
        snap.sentAt = QDateTime::fromMSecsSinceEpoch(object["sts"].toDouble());
        snap.openedAt = QDateTime::fromMSecsSinceEpoch(object["ts"].toDouble());
        snap.path = getFilePath(snap);

        if (snap.id.isEmpty()) {
            qWarning() << "invalid snap in data:" << object << snap.id << snap.sender;
            continue;
        }

        int snapIndex = m_snaps.indexOf(snap);
        if (snapIndex == -1) {
            beginInsertRows(QModelIndex(), m_snaps.length(), m_snaps.length());
            m_snaps.append(snap);
            endInsertRows();
        } else {
            m_snaps[snapIndex] = snap;
            emit dataChanged(index(snapIndex), index(snapIndex));
        }

        if (!snap.id.isEmpty() && !snap.sender.isEmpty()) {
            qDebug() << "checking if exists" << snap.path << QFileInfo(snap.path).exists();
            if (!QFileInfo(snap.path).exists() && snap.openedAt < QDateTime::currentDateTime()) {
                qDebug() << "need to download";
                emit needSnapBlob(snap.id);
            }
        }
        if (QFileInfo(snap.path).exists()) {
            snap.downloaded = true;
        }
    }
}
