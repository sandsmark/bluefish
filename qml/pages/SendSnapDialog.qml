import QtQuick 2.0
import Sailfish.Silica 1.0


Dialog {
    id: page

    property var selectedFriends: []
    property string filename
    property int time: 10

    canAccept: true//selectedFriends.length > 0
    //acceptDestinationAction: PageStackAction.Pop
    //acceptDestination: snapList
    onAccepted: {
        console.log(filename + " to " + selectedFriends)
        if (selectedFriends.length < 1) {
            return
        }

        Snapchat.sendSnap({
                              "file": filename,
                              "recipients": selectedFriends,
                              "time": time
                          })
    }

    DialogHeader {
        id: header
        acceptText: "Send"
    }

    SilicaListView {
        id: listView
        model: FriendsModel
        anchors {
            top: header.bottom
            right: parent.right
            left: parent.left
            bottom: parent.bottom
        }


        delegate: BackgroundItem {
            id: delegate

            TextSwitch {
                id: userSelect
                text: displayName === "" ? username : displayName
                description: displayName === "" ? "" : username
                onCheckedChanged: {
                    if (checked) {
                        selectedFriends.push(username)
                        console.log("sending to: " + selectedFriends)
                    } else {
                        selectedFriends.splice(selectedFriends.indexOf(username), 1)
                    }
                }
            }
        }
        VerticalScrollDecorator {}
    }
}
