#include "camerahelper.h"

#include <QMediaObject>
#include <QVideoEncoderSettings>
#include <QVideoEncoderSettingsControl>
#include <QMediaService>

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
