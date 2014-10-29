#include "friendsmodel.h"

#include <QJsonObject>

FriendsModel::FriendsModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QHash<int, QByteArray> FriendsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[UsernameRole] = "username";
    names[DisplayNameRole] = "displayName";
    return names;
}

void FriendsModel::parseJson(const QJsonArray &friends)
{
    //beginResetModel();
    //m_friends.clear();
    int added = 0;
    foreach (const QJsonValue &item, friends) {
        QJsonObject object = item.toObject();

        QString username = object["name"].toString();
        QString displayName = object["display"].toString();
        if (displayName.isNull()) displayName = "";
        if (username.isEmpty()) {
            qWarning() << "invalid user in friend array" << object;
            continue;
        }
        m_friends.append(Friend());
        m_friends.last().username = username;
        m_friends.last().displayName = displayName;
        added++;
    }
    beginInsertRows(QModelIndex(), 0, added - 1);
    endInsertRows();

    //endResetModel();
}

QVariant FriendsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() > m_friends.size() || index.row() < 0) {
        qWarning() << "asked for invalid row" << index.row();
        return QVariant();
    }

    switch (role) {
    case UsernameRole:
        return m_friends[index.row()].username;
        break;
    case DisplayNameRole:
        return m_friends[index.row()].displayName;
    default:
        qWarning() << "unknown role" << role;
        return QVariant();
    }
}
