import QtQuick
import QtQuick.Controls
import EngineeringSupporterTools

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Engineering Supporter Tools")

    Backend { id: backend }
    SqlDatabase { id: sqlDb }
    KeyValueStore { id: kvs }
    GitHubUpdater { id: updater }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        Column {
            anchors.centerIn: parent
            spacing: 15
            width: parent.width * 0.8

            Text {
                text: qsTr("Production Ready Structure")
                font.pixelSize: 22
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: backend.message
                font.pixelSize: 16
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Button {
                text: qsTr("Update Message (C++)")
                width: parent.width
                onClicked: backend.updateMessage()
            }

            Button {
                text: qsTr("Test SQLite")
                width: parent.width
                onClicked: {
                    if (sqlDb.openDatabase("app_master.db")) {
                        backend.setMessage("SQLite: Connected!")
                    }
                }
            }

            Button {
                text: qsTr("Test Redis-like KVS")
                width: parent.width
                onClicked: {
                    kvs.set("user.name", "Junie")
                    backend.setMessage("KVS: Set user.name to " + kvs.get("user.name"))
                }
            }

            Button {
                text: qsTr("Check for Updates (GitHub)")
                width: parent.width
                onClicked: updater.checkForUpdates("user/repo")
            }
        }
    }
}
