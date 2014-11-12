#ifndef CAMERAHELPER_H
#define CAMERAHELPER_H

#include <QObject>

class CameraHelper : public QObject
{
    Q_OBJECT
public:
    explicit CameraHelper(QObject *parent = 0);

signals:

public slots:
    void setCamera(QObject *camera);
    QString rotateImage(const QString file);

};

#endif // CAMERAHELPER_H
