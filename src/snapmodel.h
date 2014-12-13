#ifndef SNAPMODEL_H
#define SNAPMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "snap.h"

class SnapModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        RecipientRole,
        SenderRole,
        TimeoutRole,
        StatusRole,
        ScreenshotCountRole,
        MediaTypeRole,
        SentAtRole,
        OpenedAtRole,
        PathRole,
        DownloadedRole
    };

    Q_ENUMS(SnapStatus)

    explicit SnapModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_snaps.size(); }

    void parseJson(const QJsonArray &snaps);

signals:
    void needSnapBlob(const Snap &id);

public slots:
    void snapDownloaded(const Snap &snap);

private:
    QList<Snap> m_snaps;

};

#endif // SNAPMODEL_H
