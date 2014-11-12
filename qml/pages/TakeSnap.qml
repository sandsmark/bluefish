/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0

Page {
    id: page
    property string path

    property bool imageMode: true
    property bool recording: false
    function sendSnap(filename) {
        console.log("filename: " + filename)
        var sendDialog = window.pageStack.push(Qt.resolvedUrl("PrepareSnapDialog.qml"), { "filename": CameraHelper.rotateImage(filename) } )
        //sendDialog.accepted.connect(function() {
        //    window.pageStack.navigateForward(PageStackAction.Immediate)
        //})
    }

    function stopRecording() {
        camera.videoRecorder.stop()
        recording = false
        sendSnap("/tmp/snapVideo.mp4")
    }

    Camera {
        id: camera
        captureMode: imageMode ? Camera.CaptureStillImage : Camera.CaptureVideo

        imageCapture {
            onImageCaptured: camera.cameraState = Camera.UnloadedState // try to not get the camera stuck...
        }

        videoRecorder{
            resolution: Qt.size(1280, 720)
            audioSampleRate: 48000
            audioBitRate: 96
            audioChannels: 1
            audioCodec: "audio/mpeg, mpegversion=(int)4"

            frameRate: 25
            videoCodec: "video/x-h264"
            mediaContainer: "video/x-matroska"

            onRecorderStateChanged: {
                if (camera.videoRecorder.recorderState == CameraRecorder.StoppedState) {
                    console.log("saved to: " + camera.videoRecorder.outputLocation)
                }
            }

             outputLocation: "/tmp/snapVideo.mp4"
        }

        Component.onCompleted: {
            CameraHelper.setCamera(camera)
        }

    }

    Component.onDestruction: {
        camera.cameraState = Camera.UnloadedState // try to not get the camera stuck...
    }


    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: imageMode ? qsTr("Take video") : qsTr("Take image")
                onClicked: imageMode = !imageMode
            }
        }

        Column {
            width: parent.width

            VideoOutput {
                width: parent.width
                height: Screen.height
                orientation: 90
                source: camera

                Rectangle {
                    anchors.fill: snapButton
                    color: "black"
                    opacity: 0.2
                }

                Button {
                    id: snapButton
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter

                    text: imageMode ? qsTr("Take picture") : (recording ? qsTr("Stop recording") : qsTr("Start recording"))
                    onClicked: {
                        if (imageMode) {
                            camera.imageCapture.captureToLocation("/tmp/snapImage.jpg")
                            sendSnap("/tmp/snapImage.jpg")
                        } else {
                            if (recording) {
                                camera.videoRecorder.stop()
                                recording = false
                                stopRecordingPopup.cancel()
                            } else {
                                recording = true
                                camera.videoRecorder.record()
                                stopRecordingPopup.execute(
                                            qsTr("Recording ending"),
                                            stopRecording,
                                            10000
                                            )
                            }
                        }
                    }
                }
            }
        }
    }

    RemorsePopup {
        id: stopRecordingPopup
        onCanceled: {
            stopRecording()
        }
    }
}
