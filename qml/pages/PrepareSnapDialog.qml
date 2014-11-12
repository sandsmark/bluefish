import QtQuick 2.0
import Sailfish.Silica 1.0

Dialog {
    id: page

    property string filename

    canAccept: true
    onAccepted: {
        console.log("fleeeeeeeeeeeeh")
        var sendDialog = window.pageStack.push(Qt.resolvedUrl("SendSnapDialog.qml"), { "filename": filename } )
    }
    onDone: {
        console.log("done")
    }

    Image {
        anchors.fill: parent
        source: "file://" + filename
    }

    Rectangle {
        id: textBackground
        color: "black"
        opacity: 0.5
        visible: textInput.focus || (textInput.text.length > 0)
        width: parent.width
        height: 50

        property int targetY: parent.height / 1.5
        y: textInput.focus ? parent.height / 2 : targetY
    }

    TextInput {
        id: textInput
        anchors.fill: textBackground
        color: "white"
        font.bold: true
        horizontalAlignment: TextInput.AlignHCenter
        verticalAlignment: TextInput.AlignVCenter
    }

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        property int origY

        onPressed: {
            origY = mouse.y
        }

        onPositionChanged: {
            textBackground.targetY = mouse.y
        }

        onClicked: {
            mouse.accepted = false
            if (Math.abs(mouse.y - origY) > 5) {
                return
            }


            if (textInput.focus) {
                console.log("removing focus")
                textInput.focus = false
                mouse.accepted = true
            } else if (textInput.text === "" || (mouse.y > textBackground.y && mouse.y < textBackground.y + textBackground.height)) {
                console.log("giving focus")
                textInput.forceActiveFocus()
                mouse.accepted = true
            }
        }
    }
    DialogHeader {
        anchors.top: parent.top
        acceptText: qsTr("Select recipients")
    }
}
