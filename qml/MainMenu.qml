import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "transparent"
    
    signal playCampaignClicked()
    signal customLevelClicked()

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#11111b" }
            GradientStop { position: 1.0; color: "#1e1e2e" }
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 40

        Text {
            text: "Temple Trap"
            color: "#cdd6f4"
            font.pixelSize: 48
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            text: "Play Campaign"
            Layout.alignment: Qt.AlignHCenter
            onClicked: root.playCampaignClicked()
            
            contentItem: Text {
                text: parent.text
                color: "#11111b"
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                implicitWidth: 250
                implicitHeight: 50
                radius: 8
                color: parent.pressed ? "#b4befe" : "#cba6f7"
                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }

        Button {
            text: "Custom Level Builder"
            Layout.alignment: Qt.AlignHCenter
            onClicked: root.customLevelClicked()
            
            contentItem: Text {
                text: parent.text
                color: "#11111b"
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                implicitWidth: 250
                implicitHeight: 50
                radius: 8
                color: parent.pressed ? "#a6e3a1" : "#94e2d5"
                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }
    }
}
