import QtQuick

Rectangle {
    id: boardRoot
    width: 320
    height: 320
    color: "#313244"
    radius: 12
    border.color: "#45475a"
    border.width: 2

    // The water (empty space)
    Rectangle {
        width: 100
        height: 100
        x: ((gameModel.waterPos - 1) % 3) * 100 + 10
        y: Math.floor((gameModel.waterPos - 1) / 3) * 100 + 10
        color: "#89b4fa"
        radius: 8
        opacity: 0.3
        
        Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
        Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutCubic } }
    }

    // The exit
    Rectangle {
        width: 20
        height: 100
        x: -10
        y: 10
        color: "#a6e3a1"
        radius: 4
    }

    // Tiles
    Repeater {
        model: gameModel.tiles
        
        Tile {
            tileData: modelData
        }
    }

    // Pawn
    Rectangle {
        width: 40
        height: 40
        radius: 20
        color: "#f38ba8"
        border.color: "#11111b"
        border.width: 2
        
        // Cell position
        property int cellPos: gameModel.pawnPos
        
        x: cellPos === 0 ? -10 : ((cellPos - 1) % 3) * 100 + 40
        y: cellPos === 0 ? 40 : Math.floor((cellPos - 1) / 3) * 100 + 40
        
        Behavior on x { NumberAnimation { duration: 250; easing.type: Easing.OutBack } }
        Behavior on y { NumberAnimation { duration: 250; easing.type: Easing.OutBack } }
        z: 10
    }
    
    // Interactions
    MouseArea {
        anchors.fill: parent
        onClicked: (mouse) => {
            let col = Math.floor((mouse.x - 10) / 100)
            let row = Math.floor((mouse.y - 10) / 100)
            if (col >= 0 && col < 3 && row >= 0 && row < 3) {
                let clickedPos = row * 3 + col + 1
                gameModel.movePawn(clickedPos)
                gameModel.moveTile(clickedPos)
            }
        }
    }
}
