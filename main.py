import sys
import os
import json
from pathlib import Path

build_dir = Path(__file__).parent / "build"
sys.path.append(str(build_dir))


import temple_trap_core as tt
from PySide6.QtCore import QObject, Property, Signal, Slot, QUrl
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine


class GameModel(QObject):
    stateChanged = Signal()
    pawnPosChanged = Signal()
    waterPosChanged = Signal()
    isGoalChanged = Signal()
    messageChanged = Signal()
    currentLevelChanged = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)

        self._message = "Welcome to Temple Trap!"

        # Load levels from JSON
        levels_path = os.path.join(os.path.dirname(__file__), "levels.json")

        with open(levels_path, "r") as f:
            data = json.load(f)

        self.levels = []

        for level in data["levels"]:
            input_data = tuple(tuple(tile) for tile in level["tiles"])
            pawn_pos = level["pawn"]

            controller = tt.GameController(input_data, pawn_pos)
            self.levels.append(controller.get_current_state())

        # Start from first level
        self._current_level = 0
        self.controller = tt.GameController(
            tuple(tuple(tile) for tile in data["levels"][0]["tiles"]),
            data["levels"][0]["pawn"]
        )

        self._sync_state()

    @Slot(list, int, result=bool)
    def load_custom_level(self, tile_configs, pawn_pos):
        if hasattr(tile_configs, "toVariant"):
            tile_configs = tile_configs.toVariant()
            
        input_data = [(0, 0)] * 10
        for i in range(10):
            input_data[i] = (0, 0)
            
        input_data[0] = (0, 0) # Goal is always 0
            
        used_pos = set()
        for config in tile_configs:
            tid = config.get('id')
            pos = config.get('pos')
            orient = config.get('orientation', 1)
            input_data[tid] = (pos, orient)
            used_pos.add(pos)

        # Infer Water
        for pos in range(1, 10):
            if pos not in used_pos:
                input_data[9] = (pos, 0)
                break

        # Re-initialize controller
        new_controller = tt.GameController(tuple(input_data), pawn_pos)
        
        # Verify it is solvable
        initial_state = new_controller.get_current_state()
        path = [initial_state]
        temp_controller = tt.GameController(tuple(input_data), pawn_pos)
        
        is_solvable = temp_controller.is_goal()
        if not is_solvable:
            hint = temp_controller.get_hint()
            if hint is not None:
                is_solvable = True

        if not is_solvable:
            return False

        self.controller = new_controller
        self.levels = [initial_state]
        self._current_level = 1
        self.message = "Custom Level Start!"
        self._sync_state()
        self.currentLevelChanged.emit()
        return True

    def load_level(self, level):
        if level <= len(self.levels):
            self._current_level = level
            self.controller.set_state(self.levels[level - 1])
            self.message = f"Level {level} Start!"
            self._sync_state()
            self.currentLevelChanged.emit()
            
    @Slot()
    def next_level(self):
        if self._current_level < len(self.levels):
            self.load_level(self._current_level + 1)
        else:
            self.message = "You finished all levels!"

    @Property(int, notify=currentLevelChanged)
    def currentLevel(self):
        return self._current_level

    @Property(int, notify=currentLevelChanged)
    def totalLevels(self):
        return len(self.levels)

    def _sync_state(self):
        state = self.controller.get_current_state()
        
        self._pawn_pos = state.pawn_pos
        self._water_pos = state.water_pos
        self._is_goal = self.controller.is_goal()
        
        tiles_list = []
        orientations = {
            tt.TileNames.Goal: 0,
            tt.TileNames.A: 1, tt.TileNames.B: 3, tt.TileNames.C: 3,
            tt.TileNames.D: 3, tt.TileNames.E: 3, tt.TileNames.F: 2,
            tt.TileNames.G: 4, tt.TileNames.H: 2, tt.TileNames.Water: 0
        }
        
        enum_to_id = {
            tt.TileNames.Goal: 0,
            tt.TileNames.A: 1, tt.TileNames.B: 2, tt.TileNames.C: 3,
            tt.TileNames.D: 4, tt.TileNames.E: 5, tt.TileNames.F: 6,
            tt.TileNames.G: 7, tt.TileNames.H: 8, tt.TileNames.Water: 9
        }
        
        for pos in range(1, 10):
            t_name = state.tiles[pos]
            if t_name != tt.TileNames.Water:
                tiles_list.append({
                    'id': enum_to_id.get(t_name, 0),
                    'pos': pos,
                    'orientation': orientations.get(t_name, 0)
                })
        
        self._tiles = tiles_list
        self.stateChanged.emit()
        self.pawnPosChanged.emit()
        self.waterPosChanged.emit()
        self.isGoalChanged.emit()

    @Property(list, notify=stateChanged)
    def tiles(self):
        return self._tiles
        
    @Property(int, notify=pawnPosChanged)
    def pawnPos(self):
        return self._pawn_pos

    @Property(int, notify=waterPosChanged)
    def waterPos(self):
        return self._water_pos

    @Property(bool, notify=isGoalChanged)
    def isGoal(self):
        return self._is_goal
        
    @Property(str, notify=messageChanged)
    def message(self):
        return self._message

    @message.setter
    def message(self, value):
        if self._message != value:
            self._message = value
            self.messageChanged.emit()

    @Slot(int)
    def moveTile(self, from_pos):
        if self.controller.move_tile(from_pos):
            self.message = "Tile moved."
            self._sync_state()
        else:
            self.message = "Invalid tile move!"

    @Slot(int)
    def movePawn(self, to_pos):
        if self.controller.move_pawn(to_pos):
            self.message = "Pawn moved."
            self._sync_state()
        else:
            self.message = "Invalid pawn move!"

    @Slot()
    def hint(self):
        hint_state = self.controller.get_hint()
        if hint_state is not None:
            self.controller.set_state(hint_state)
            self.message = "Hint applied."
            self._sync_state()
        else:
            if self.controller.is_goal():
                self.message = "Already at goal!"
            else:
                self.message = "No path found!"

if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    engine = QQmlApplicationEngine()
    
    model = GameModel()
    engine.rootContext().setContextProperty("gameModel", model)
    
    qml_file = Path(__file__).parent / "qml" / "Main.qml"
    engine.load(QUrl.fromLocalFile(str(qml_file)))
    
    if not engine.rootObjects():
        sys.exit(-1)
        
    sys.exit(app.exec())
