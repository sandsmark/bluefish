#ifndef FRIENDSMODEL_H
#define FRIENDSMODEL_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QDebug>


class FriendsModel : public QAbstractListModel
{
    Q_OBJECT

    struct Friend {
        QString username;
        QString displayName;
    };

    enum Roles {
        UsernameRole,
        DisplayNameRole
    };

public:
    explicit FriendsModel(QObject *parent = 0);
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex & = QModelIndex()) const { return m_friends.size(); }
    void parseJson(const QJsonArray &friends);
    QVariant data(const QModelIndex &index, int role) const;

signals:

public slots:

private:
    QVector<Friend> m_friends;
};

#endif // FRIENDSMODEL_H
