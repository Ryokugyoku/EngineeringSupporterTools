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
    GitHubUpdater { 
        id: updater 
        
        onUpdateAvailable: (newVersion, downloadUrl) => {
            updateDialog.newVersion = newVersion
            updateDialog.downloadUrl = downloadUrl
            updateDialog.state = "Available"
            updateDialog.open()
        }
        onDownloadFinished: (filePath) => {
            updateDialog.state = "ReadyToInstall"
        }
        onErrorOccurred: (message) => {
            backend.setMessage("Update Error: " + message)
        }
    }

    Component.onCompleted: {
        // 起動時に自動でアップデートを確認
        updater.checkForUpdates("Ryokugyoku/EngineeringSupporterTools")
    }

    Dialog {
        id: updateDialog
        property string newVersion: ""
        property string downloadUrl: ""
        property string state: "Available" // Available, Downloading, ReadyToInstall

        title: state === "Available" ? "Update Available" : 
               state === "Downloading" ? "Downloading..." : "Update Ready"
        
        anchors.centerIn: parent
        modal: true
        standardButtons: state === "Downloading" ? Dialog.NoButton : 
                         state === "Available" ? Dialog.Ok | Dialog.Cancel : Dialog.Ok

        Column {
            spacing: 10
            width: parent.width
            
            Text { 
                text: state === "Available" ? "A new version is available: " + updateDialog.newVersion :
                      state === "Downloading" ? "Downloading update..." : "Download complete!"
            }
            
            ProgressBar {
                visible: updateDialog.state === "Downloading"
                value: updater.downloadProgress
                width: parent.width
            }

            Text { 
                visible: updateDialog.state === "Available"
                text: "Do you want to download and install it?"
                font.italic: true
            }
        }

        onAccepted: {
            if (state === "Available") {
                state = "Downloading"
                updater.downloadUpdate(updateDialog.downloadUrl)
                updateDialog.open() // Keep open
            } else if (state === "ReadyToInstall") {
                updater.installUpdate()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        Column {
            anchors.centerIn: parent
            spacing: 15
            width: parent.width * 0.8

            Text {
                text: qsTr("Engineering Supporter")
                font.pixelSize: 22
                font.bold: true
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "Current Version: " + updater.currentVersion
                font.pixelSize: 12
                color: "#666666"
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
                highlighted: true
                onClicked: {
                    backend.setMessage("Checking for updates...")
                    updater.checkForUpdates("Ryokugyoku/EngineeringSupporterTools")
                }
            }
        }
    }
}
