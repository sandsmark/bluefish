#include "camerahelper.h"

#include <QMediaObject>
#include <QVideoEncoderSettings>
#include <QVideoEncoderSettingsControl>
#include <QMediaService>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QPainter>

CameraHelper::CameraHelper(QObject *parent) :
    QObject(parent)
{
}

void CameraHelper::setCamera(QObject *camera)
{
    QMediaObject *mediaObject = camera
            ? qobject_cast<QMediaObject *>(camera->property("mediaObject").value<QObject *>())
            : 0;

    if (mediaObject && mediaObject->service()) {
        QVideoEncoderSettingsControl *videoEncoder = mediaObject->service()->requestControl<QVideoEncoderSettingsControl *>();
        if (videoEncoder) {
            QVideoEncoderSettings settings = videoEncoder->videoSettings();
            settings.setEncodingOption(QStringLiteral("preset"), QStringLiteral("high"));
            videoEncoder->setVideoSettings(settings);
        }
    } else {
        qWarning() << "unable to get camera for setting quality";
    }
}

QString CameraHelper::rotateImage(const QString file)
{
    // Because stupid camera saves it wrong
    QImage image(file);
/*    QTransform transform;
    transform.rotate(90);
    image.transformed(transform).save(file);*/
    // Create new rotatedPixmap that size is same as original
    /*QPixmap rotatedPixmap(image.width(), image.height());

    // Create a QPainter for it
    QPainter p(&rotatedPixmap);

    // Set rotation origo into pixmap center
    QSize size = image.size();
    p.translate(size.height()/2,size.height()/2);

    // Rotate the painter 90 degrees
    p.rotate(90);

    // Set origo back to upper left corner
    p.translate(-size.height()/2,-size.height()/2);

    // Draw your original pixmap on it
    p.drawImage(0, 0, image);
    p.end();

    // Change original pixmap reference into new rotated pixmap
    rotatedPixmap.toImage().save(file);*/

    QPixmap pixmap(file);
    QTransform transform;
    transform.translate(pixmap.width()/2, pixmap.height()/2);
    transform.rotate(90);
    transform.translate(-pixmap.width()/2, -pixmap.height()/2);
    pixmap = pixmap.transformed(transform);
    pixmap.save(file);
    return file;
}
