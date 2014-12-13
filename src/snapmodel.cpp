#include <QJsonObject>
#include <QDebug>

#include "snapmodel.h"
#include "snapchat.h"
#include "common.h"

SnapModel::SnapModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void SnapModel::snapDownloaded(const Snap &snap)
{
    qDebug() << "snap downloaded" << snap.id();
    int snapIndex = m_snaps.indexOf(snap);
    if (snapIndex == -1) {
        qWarning() << "downloaded snap that doesn't exist";
        return;
    }
    emit dataChanged(index(snapIndex), index(snapIndex));
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
        return snap.id();
    case RecipientRole:
        return snap.recipient();
    case SenderRole:
        return snap.sender();
    case TimeoutRole:
        return snap.timeout();
    case StatusRole:
        return snap.status();
    case ScreenshotCountRole:
        return snap.screenshots();
    case MediaTypeRole:
        return snap.type();
    case SentAtRole: {
        QDateTime now = QDateTime::currentDateTime();
        int daysAgo = snap.sentAt().daysTo(now);
        if (daysAgo > 1) {
            return tr("%1 days ago").arg(daysAgo);
        }
        int secsAgo = snap.sentAt().secsTo(now);
        if (secsAgo > 3600) {
            return tr("%1 hours ago").arg(secsAgo / 3600);
        }
        if (secsAgo > 60) {
            return tr("%1 minutes ago").arg(secsAgo / 60);
        }
        return tr("%1 seconds ago").arg(secsAgo);
    }
    case OpenedAtRole:
        return snap.openedAt();
    case PathRole:
        return snap.path();
    case DownloadedRole:
        return snap.downloaded();
    default:
        return QVariant();
    }
}

void SnapModel::parseJson(const QJsonArray &snaps)
{
    foreach(const QJsonValue item, snaps) {
        QJsonObject object = item.toObject();

        if (object.contains("c_id")) {
            qDebug() << "skipping sent snap";
            continue;
        }

        if (!object.contains("id")) {
            qDebug() << "skipping snap without id";
            continue;
        }

        if (!object.contains("sn")) {
            qDebug() << "skipping snap without sender";
            continue;
        }

        Snap snap(object.toVariantMap());

        if (snap.status() != Snap::Delivered) {
            qDebug() << "not handling snap that is not delivered";
            continue;
        }

        if (snap.type() != Snap::Image && snap.type() != Snap::Video && snap.type() != Snap::VideoNoAudio) {
            qDebug() << "unrecognized mediatype";
            continue;
        }

        // If the snap isn't downloaded, and still at snapchat's servers (it is there for 30 days)
        if (!snap.downloaded() && snap.sentAt().daysTo(QDateTime::currentDateTime()) > 30) {
            qDebug() << "too old snap, no data" << snap.id();
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

        if (!snap.downloaded()) {
            qDebug() << "need to download";
            emit needSnapBlob(snap);
        }
    }
}
