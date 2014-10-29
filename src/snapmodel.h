#ifndef SNAPMODEL_H
#define SNAPMODEL_H

#include <QAbstractListModel>
#include <QDateTime>
#include <QJsonArray>

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

    struct Snap {
        QString id;
        QString recipient;
        QString sender;
        int timeout;
        SnapStatus status;
        int screenshots;
        MediaType type;
        QDateTime sentAt;
        QDateTime openedAt;
        bool downloaded;
        QString path;
    };

    Q_ENUMS(SnapStatus)

    explicit SnapModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_snaps.size(); }

    void parseJson(const QJsonArray &snaps);
    QString getFilePath(const Snap &snap);

    Snap *getSnap(QString id);

signals:
    void needSnapBlob(QString id);

public slots:
    void snapDownloaded(QString id);
    void snapGone(QString id);

private:
    QList<Snap> m_snaps;

};

#endif // SNAPMODEL_H
