#ifndef CAMERAHELPER_H
#define CAMERAHELPER_H

#include <QObject>
#include <QUuid>

class CameraHelper : public QObject
{
    Q_OBJECT
public:
    explicit CameraHelper(QObject *parent = 0);

signals:

public slots:
    void setCamera(QObject *camera);
    QString rotateImage(const QString file);
    QString createUuid() {
        QString uuid(QUuid::createUuid().toString());
        uuid.chop(1);
        uuid.remove(0, 1);
        return uuid;
    }

};

#endif // CAMERAHELPER_H
