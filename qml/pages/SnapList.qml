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
import org.nemomobile.thumbnailer 1.0

Page {
    id: snaplist


    onStatusChanged: {
        if (status === PageStatus.Active) {
            window.pageStack.pop()
        }
    }

    SilicaListView {
        id: listView
        model: SnapModel
        anchors.fill: parent

        PullDownMenu {
            busy: Snapchat.isBusy
            MenuItem {
                text: qsTr("Refresh")
                onClicked: Snapchat.getUpdates()
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
            }
        }

        header: PageHeader {
            title: qsTr("Received snaps")
        }
        delegate: BackgroundItem {
            id: delegate

            Row {
                Thumbnail {
                    id: snapPreview
                    width: infoLabels.height
                    height: infoLabels.height
                    source: (downloaded) ? filePath : ""
                    fillMode: Image.PreserveAspectCrop
                    priority: Thumbnail.NormalPriority
                    sourceSize.height: height
                    sourceSize.width: width

                    BusyIndicator {
                        id: imageDownloadingIndicator
                        anchors.fill: parent
                        running: !downloaded && (snapPreview.status === Image.Error)
                    }

                    Label {
                        id: label
                        anchors.centerIn: parent
                        text: "video"
                        visible: (type !== 0)
                        font.pointSize: 5
                    }
                    onStatusChanged: {
                        if (status === Image.Error) {
                            label.text = "invalid"
                            label.color = "red"
                            label.visible = true
                        }
                    }
                }
                /*Image {
                        id: snapPreview
                        width: infoLabels.height
                        height: infoLabels.height
                        source: (downloaded && type == 0) ? filePath : ""
                        fillMode: Image.PreserveAspectCrop

                        BusyIndicator {
                            id: imageDownloadingIndicator
                            anchors.fill: parent
                            running: !downloaded && (snapPreview.status === Image.Error)
                        }

                        Label {
                            id: label
                            anchors.centerIn: parent
                            text: "video"
                            visible: (type !== 0)
                            font.pointSize: 5
                        }
                        onStatusChanged: {
                            if (status === Image.Error) {
                                label.text = "invalid"
                                label.color = "red"
                                label.visible = true
                            }
                        }
                    }*/


                Column {
                    id: infoLabels
                    Label {
                        id: senderLabel
                        x: Theme.paddingSmall
                        text: sender
                        color: delegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                    }
                    Label {
                        id: timestampLabel
                        x: Theme.paddingLarge
                        text: filePath//sentAt
                        font.pointSize: Theme.fontSizeSmall
                    }
                }

            }
            onClicked: {
                if (type === 0) {
                    window.pageStack.push(Qt.resolvedUrl("ImageView.qml"), {path: filePath})
                } else {
                    window.pageStack.push(Qt.resolvedUrl("VideoView.qml"), {path: filePath})
                }
            }
        }
        VerticalScrollDecorator {}
    }
}


