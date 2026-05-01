import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 600
    height: 700
    title: "Temple Trap"
    color: "#1e1e2e"

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: MainMenu {
            onPlayCampaignClicked: stackView.push("GameScreen.qml")
            onCustomLevelClicked: stackView.push("LevelEditor.qml")
        }
    }
}
