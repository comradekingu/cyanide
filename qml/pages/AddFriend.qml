import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page
    allowedOrientations: Orientation.All
    Component.onCompleted: {
        pages.push("AddFriend.qml")
    }
    Component.onDestruction: {
        pages.pop()
    }

    TextField {
        id: toxID
        width: parent.width
        height: implicitHeight
        y: 2 * Theme.paddingLarge

        inputMethodHints: Qt.ImhNoPredictiveText + Qt.ImhNoAutoUppercase + Qt.ImhEmailCharactersOnly
        focus: true
        placeholderText: "Tox ID"

        EnterKey.onClicked: {
            focus = false
            message.focus = true
        }
    }
    TextArea {
        id: message
        width: parent.width
        //height: Math.max(page.width/3, implicitHeight)
        height: implicitHeight
        y: 3 * Theme.paddingLarge + toxID.height

        inputMethodHints: Qt.ImhNoAutoUppercase
        // TODO use DEFAULT_FRIEND_REQUEST_MESSAGE
        //: default friend request message - feel free to tranlate freely
        placeholderText: qsTr("Tox me maybe?")
    }
    Button {
        id: button
        text: qsTr("Send friend request")
        enabled: true
        onClicked: submit()
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: message.bottom
        anchors.topMargin: Theme.paddingLarge
    }
    function submit() {
        var errmsg = cyanide.send_friend_request(toxID.text, message.text)
        if(errmsg === "") {
            pageStack.pop()
        } else {
            cyanide.notify_error(qsTr("Failed to send friend request"), qsTr(errmsg))
        }
    }
}
