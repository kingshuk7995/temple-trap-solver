import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "transparent"

    property int selectedTileId: -1 // 1-8 for tiles, 100 for pawn
    property var placedTiles: [] // elements: {id, pos, orientation}
    property int pawnPos: -1
    property string errorMessage: ""

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
        spacing: 20

        Text {
            text: "Custom Level Builder"
            color: "#cdd6f4"
            font.pixelSize: 32
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        Text {
            text: errorMessage
            color: "#f38ba8"
            font.pixelSize: 16
            Layout.alignment: Qt.AlignHCenter
        }

        // Board area
        Rectangle {
            width: 320
            height: 320
            color: "#313244"
            radius: 12
            border.color: "#45475a"
            border.width: 2
            Layout.alignment: Qt.AlignHCenter

            // Grid lines
            Grid {
                anchors.centerIn: parent
                columns: 3
                spacing: 0
                Repeater {
                    model: 9
                    Rectangle {
                        width: 100
                        height: 100
                        color: "transparent"
                        border.color: "#45475a"
                        border.width: 1
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                let clickedPos = index + 1
                                // if something is already here, select it or rotate it
                                let tileIdx = placedTiles.findIndex(t => t.pos === clickedPos)
                                if (tileIdx !== -1) {
                                    // Rotate
                                    let newTiles = placedTiles.slice()
                                    newTiles[tileIdx].orientation = (newTiles[tileIdx].orientation % 4) + 1
                                    placedTiles = newTiles
                                    return
                                }
                                if (pawnPos === clickedPos) {
                                    // Remove pawn
                                    pawnPos = -1
                                    return
                                }
                                
                                // Place selected
                                if (selectedTileId === 100) {
                                    if (pawnPos !== -1) {
                                        errorMessage = "Pawn already placed!"
                                        return
                                    }
                                    pawnPos = clickedPos
                                    selectedTileId = -1
                                } else if (selectedTileId !== -1) {
                                    let isPlaced = placedTiles.some(t => t.id === selectedTileId)
                                    if (isPlaced) {
                                        errorMessage = "Tile already placed!"
                                        return
                                    }
                                    let newTiles = placedTiles.slice()
                                    newTiles.push({id: selectedTileId, pos: clickedPos, orientation: 1})
                                    placedTiles = newTiles
                                    selectedTileId = -1
                                }
                            }
                            onDoubleClicked: {
                                let clickedPos = index + 1
                                let newTiles = placedTiles.filter(t => t.pos !== clickedPos)
                                placedTiles = newTiles
                            }
                        }
                    }
                }
            }

            // Placed tiles
            Repeater {
                model: placedTiles
                Tile {
                    tileData: modelData
                }
            }
            
            // Pawn
            Rectangle {
                visible: pawnPos !== -1
                width: 40
                height: 40
                radius: 20
                color: "#f38ba8"
                border.color: "#11111b"
                border.width: 2
                x: pawnPos !== -1 ? ((pawnPos - 1) % 3) * 100 + 40 : 0
                y: pawnPos !== -1 ? Math.floor((pawnPos - 1) / 3) * 100 + 40 : 0
                z: 10
            }
            
            // Exit indicator
            Rectangle {
                width: 20
                height: 100
                x: -10
                y: 10
                color: "#a6e3a1"
                radius: 4
            }
        }

        // Palette
        Text {
            text: "Select a piece to place (Click to place, double-click to remove, click placed to rotate)"
            color: "#a6adc8"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 10
            
            Repeater {
                model: 8 // Tiles 1-8 (A-H)
                Rectangle {
                    width: 50
                    height: 50
                    radius: 4
                    color: selectedTileId === index + 1 ? "#a6e3a1" : "transparent"
                    border.color: "#45475a"
                    
                    opacity: placedTiles.some(t => t.id === index + 1) ? 0.3 : 1.0
                    
                    Tile {
                        anchors.centerIn: parent
                        width: 40
                        height: 40
                        scale: 0.4
                        tileData: { "id": index + 1, "pos": 1, "orientation": 1 }
                        // Hide internal position mapping to keep it centered
                        x: 0
                        y: 0
                        Behavior on x { enabled: false }
                        Behavior on y { enabled: false }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (!placedTiles.some(t => t.id === index + 1)) {
                                selectedTileId = index + 1
                            }
                        }
                    }
                }
            }
            
            // Pawn palette item
            Rectangle {
                width: 50
                height: 50
                radius: 4
                color: selectedTileId === 100 ? "#a6e3a1" : "transparent"
                border.color: "#45475a"
                
                opacity: pawnPos !== -1 ? 0.3 : 1.0

                Rectangle {
                    anchors.centerIn: parent
                    width: 20
                    height: 20
                    radius: 10
                    color: "#f38ba8"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (pawnPos === -1) {
                            selectedTileId = 100
                        }
                    }
                }
            }
        }

        Button {
            text: "Play Custom Level"
            Layout.alignment: Qt.AlignHCenter
            enabled: placedTiles.length === 8 && pawnPos !== -1
            
            onClicked: {
                errorMessage = ""
                let success = gameModel.load_custom_level(placedTiles, pawnPos)
                if (success) {
                    root.StackView.view.push("GameScreen.qml")
                } else {
                    errorMessage = "This level is unsolvable!"
                }
            }
            
            contentItem: Text {
                text: parent.text
                color: parent.enabled ? "#11111b" : "#6c7086"
                font.pixelSize: 16
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 45
                radius: 8
                color: parent.enabled ? (parent.pressed ? "#b4befe" : "#cba6f7") : "#313244"
                Behavior on color { ColorAnimation { duration: 150 } }
            }
        }
    }
}
