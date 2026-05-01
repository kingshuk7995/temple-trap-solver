import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#11111b" }
            GradientStop { position: 1.0; color: "#1e1e2e" }
        }
    }

    Button {
        text: "Back to Menu"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 20
        onClicked: root.StackView.view.pop()
        
        contentItem: Text {
            text: parent.text
            color: "#cdd6f4"
            font.pixelSize: 14
        }
        background: Rectangle {
            implicitWidth: 120
            implicitHeight: 30
            radius: 4
            color: "#313244"
            border.color: "#45475a"
        }
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "Temple Trap"
            color: "#cdd6f4"
            font.pixelSize: 36
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: gameModel.currentLevel > 0 ? ("Level " + gameModel.currentLevel + " / " + gameModel.totalLevels) : "Custom Level"
            color: "#a6adc8"
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Board {
            id: board
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: gameModel.message
            color: gameModel.isGoal ? "#a6e3a1" : "#f38ba8"
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            text: gameModel.isGoal ? (gameModel.currentLevel < gameModel.totalLevels ? "Next Level" : "Finish") : "Hint / Solve Next"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                if (gameModel.isGoal) {
                    if (gameModel.currentLevel < gameModel.totalLevels) {
                        gameModel.next_level()
                    } else {
                        root.StackView.view.pop()
                    }
                } else {
                    gameModel.hint()
                }
            }
            
            contentItem: Text {
                text: parent.text
                color: "#11111b"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 45
                radius: 8
                color: gameModel.isGoal ? "#a6e3a1" : (parent.pressed ? "#b4befe" : "#cba6f7")
                
                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }
    }
}
