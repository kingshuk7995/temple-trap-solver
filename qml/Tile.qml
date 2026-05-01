import QtQuick

Item {
    id: tileRoot
    property var tileData
    
    width: 96
    height: 96
    
    // We map pos 1-9 to x,y
    x: ((tileData.pos - 1) % 3) * 100 + 12
    y: Math.floor((tileData.pos - 1) / 3) * 100 + 12
    
    Behavior on x { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
    Behavior on y { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 0
        radius: 8
        color: "#585b70"
        border.color: "#1e1e2e"
        border.width: 2

        rotation: {
            switch(tileData.orientation) {
                case 1: return 90;
                case 2: return 180;
                case 3: return 0;
                case 4: return 270;
                default: return 0;
            }
        }
        
        Behavior on rotation { NumberAnimation { duration: 300 } }

        Rectangle {
            anchors.fill: parent
            anchors.margins: 2
            radius: 6
            color: {
                let id = tileData.id
                if (id === 1 || id === 2 || id === 3) return "#a6e3a1" // Green (Top)
                else return "#f9e2af" // Sand (Bottom)
            }
        }

        // Path drawing
        Item {
            anchors.fill: parent
            
            // L-Shape
            Rectangle {
                visible: tileData.id === 1 || tileData.id === 2 || tileData.id >= 6
                width: 50
                height: 24
                color: "#cba6f7"
                x: 44
                y: 36
            }
            Rectangle {
                visible: tileData.id === 1 || tileData.id === 2 || tileData.id >= 6
                width: 24
                height: 50
                color: "#cba6f7"
                x: 36
                y: 0
            }

            // Lane
            Rectangle {
                visible: tileData.id === 3
                width: 96
                height: 24
                color: "#cba6f7"
                x: 0
                y: 36
            }

            // Stairs
            Rectangle {
                visible: tileData.id === 4 || tileData.id === 5
                width: 96
                height: 24
                color: "#f38ba8"
                x: 0
                y: 36
            }
        }
    }
}
