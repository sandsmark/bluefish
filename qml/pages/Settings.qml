import QtQuick 2.0
import Sailfish.Silica 1.0
import QtMultimedia 5.0

Dialog {
    id: page
    property string path

    canAccept: Snapchat.isLoggedIn

    function login() {
        failLabel.visible = false
        busyIndicator.running = true
        Snapchat.username = usernameField.text
        Snapchat.password = passwordField.text
        Snapchat.login()
    }
    onCanAcceptChanged: {
        if (canAccept) {
            busyIndicator.running = false
            onTriggered: Snapchat.getUpdates()
        }
    }

    Connections {
        target: Snapchat
        onLoginFailed: {
            busyIndicator.running = false
            failLabel.visible = true
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 20

            DialogHeader {
                acceptText: "Save settings"
            }

            TextField {
                id: usernameField
                width: 480
                placeholderText: qsTr("Enter username")
                label: qsTr("Username")
                text: Snapchat.username
                EnterKey.onClicked: passwordField.focus = true
                EnterKey.enabled: text.length > 0
                EnterKey.highlighted: EnterKey.enabled
                readOnly: busyIndicator.running
            }
            TextField {
                id: passwordField
                width: 480
                placeholderText: qsTr("Enter password")
                label: qsTr("Password")
                text: Snapchat.password
                echoMode: TextInput.PasswordEchoOnEdit
                EnterKey.enabled: text.length > 0 && usernameField.text.length > 0
                EnterKey.highlighted: EnterKey.enabled
                EnterKey.onClicked: login()
                readOnly: busyIndicator.running
            }

            Button {
                text: "Login"
                onClicked: login()
                enabled: passwordField.text.length > 0 && usernameField.text.length > 0
            }
            Label {
                id: failLabel
                text: qsTr("Login failed")
                visible: false
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        width: parent.width / 2
        height: width
    }
}
