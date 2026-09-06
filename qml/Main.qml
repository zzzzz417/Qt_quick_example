import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: Qt.application.name + " " + Qt.application.version

    color: "#1e1e2e"

    Column {
        anchors.centerIn: parent
        spacing: 24

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Welcome to InnoStudio")
            font.pixelSize: 28
            font.bold: true
            color: "#cdd6f4"
        }

        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Click Me")
            font.pixelSize: 16
            onClicked: {
                statusLabel.text = qsTr("Button clicked at ") + new Date().toLocaleTimeString()
            }
        }

        Label {
            id: statusLabel
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Ready.")
            font.pixelSize: 14
            color: "#a6adc8"
        }
    }
}
