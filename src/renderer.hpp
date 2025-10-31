/**
 * @brief This file contains all the utilities for drawing the states for
 * visualization
 * @paragraph
 * this has 2 utilities
 * under namespace pieces it defines different piece designs
 * in the class RenderBoard it defines logic for rendering the states
 * The RenderBoard shall only be utilized from main
 */

#include <SFML/Graphics.hpp>
#include <array>
#include <cassert>
#include <memory>
#include <vector>

#include "board.hpp"
#include "types.hpp"

namespace pieces {

class CompositeShape : public sf::Drawable, public sf::Transformable {
  std::vector<sf::RectangleShape> parts;
  std::vector<sf::Vector2f> localOffsets;

 public:
  void addPart(const sf::RectangleShape& rect, sf::Vector2f offset) {
    parts.push_back(rect);
    localOffsets.push_back(offset);
  }

  void setPosition(const sf::Vector2f& pos) {
    sf::Transformable::setPosition(pos);
    updatePartsTransform();
  }

  void setRotation(float angle) {
    sf::Transformable::setRotation(angle);
    updatePartsTransform();
  }

 private:
  void updatePartsTransform() {
    // translate to the requested position, then rotate about the tile center
    // (100,100)
    sf::Transform transform;
    transform.translate(getPosition());
    transform.rotate(getRotation(),
                     {100.f, 100.f});  // center rotation for 200x200 tiles

    for (std::size_t i = 0; i < parts.size(); ++i) {
      sf::Vector2f worldPos = transform.transformPoint(localOffsets[i]);
      parts[i].setPosition(worldPos);
      parts[i].setRotation(getRotation());
    }
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
    for (const auto& p : parts) target.draw(p, states);
  }
};

// COLORS
/**
 * #rgba(42, 7, 6, 1)
 */
sf::Color BoardColor(42, 7, 6);
sf::Color Brown(80, 69, 19);
sf::Color LightSkin(230, 220, 170);
sf::Color Green(34, 139, 34);

/// @brief adds a border to the shape
/// @param shape
void _add_grid(CompositeShape& shape) {
  sf::RectangleShape cell(sf::Vector2f(200, 200));
  cell.setFillColor(sf::Color::Transparent);
  cell.setOutlineThickness(-1.f);
  cell.setOutlineColor(BoardColor);
  shape.addPart(cell, {0, 0});
  return;
}

// FACTORY FUNCTIONS FOR SHAPES
CompositeShape makeLShapedTop() {
  CompositeShape shape;

  sf::RectangleShape base({200, 200});
  base.setFillColor(Green);
  shape.addPart(base, {0, 0});
  sf::RectangleShape horizontal({200, 50});

  horizontal.setFillColor(Brown);
  shape.addPart(horizontal, {0, 150});

  sf::RectangleShape vertical({50, 200});
  vertical.setFillColor(Brown);
  shape.addPart(vertical, {0, 0});

  _add_grid(shape);

  return shape;
}

CompositeShape makeLShapedBottom() {
  CompositeShape shape;

  sf::RectangleShape base({200, 200});
  base.setFillColor(LightSkin);
  shape.addPart(base, {0, 0});

  sf::RectangleShape left({50, 200});
  left.setFillColor(Brown);
  shape.addPart(left, {0, 0});  // left side

  sf::RectangleShape bottom({200, 50});
  bottom.setFillColor(Brown);
  shape.addPart(bottom, {0, 150});  // bottom

  _add_grid(shape);

  return shape;
}

CompositeShape makePipeShapedTop() {
  CompositeShape shape;

  sf::RectangleShape base({200, 200});
  base.setFillColor(Green);
  shape.addPart(base, {0, 0});

  sf::RectangleShape top({200, 50});
  top.setFillColor(Brown);
  shape.addPart(top, {0, 0});

  sf::RectangleShape bottom({200, 50});
  bottom.setFillColor(Brown);
  shape.addPart(bottom, {0, 150});

  _add_grid(shape);

  return shape;
}

CompositeShape makeStairShapeBottom() {
  CompositeShape shape;

  sf::RectangleShape base({200, 200});
  base.setFillColor(LightSkin);
  shape.addPart(base, {0, 0});

  sf::RectangleShape top({200, 50});
  top.setFillColor(Brown);
  shape.addPart(top, {0, 0});

  sf::RectangleShape stair({50, 100});
  stair.setFillColor(Green);
  shape.addPart(stair, {0, 50});

  sf::RectangleShape stair_texture({10, 100});
  stair_texture.setFillColor(LightSkin);
  shape.addPart(stair_texture, {20, 50});

  sf::RectangleShape bottom({200, 50});
  bottom.setFillColor(Brown);
  shape.addPart(bottom, {0, 150});

  _add_grid(shape);

  return shape;
}

CompositeShape makeWaterShape() {
  CompositeShape shape;
  sf::RectangleShape top({200, 200});
  top.setFillColor(sf::Color::Blue);
  shape.addPart(top, {0, 0});
  _add_grid(shape);
  return shape;
}

}  // namespace pieces

/**
 * @brief RenderBoard class for handling SFML based rendering
 */
class RenderBoard {
 public:
  RenderBoard(const input_tile_data_t& tiledata)
      : window(sf::VideoMode(800, 800), "Temple Trap Solver Visualization") {
    // build configuration prototypes for each tile type (1..9)
    for (int i = 1; i <= 9; i++) {
      TileNames name = static_cast<TileNames>(i);
      switch (name) {
        case TileNames::A:
        case TileNames::B:
          configuration[i] = std::make_unique<pieces::CompositeShape>(
              pieces::makeLShapedTop());
          break;
        case TileNames::C:
          configuration[i] = std::make_unique<pieces::CompositeShape>(
              pieces::makePipeShapedTop());
          break;
        case TileNames::D:
        case TileNames::E:
          configuration[i] = std::make_unique<pieces::CompositeShape>(
              pieces::makeStairShapeBottom());
          break;
        case TileNames::F:
        case TileNames::G:
        case TileNames::H:
          configuration[i] = std::make_unique<pieces::CompositeShape>(
              pieces::makeLShapedBottom());
          break;
        case TileNames::Water:
          configuration[i] = std::make_unique<pieces::CompositeShape>(
              pieces::makeWaterShape());
          break;
        default:
          assert(false && "unexpected code path");
          break;
      }
      float rotation = RenderBoard::get_angle(tiledata[i].second);
      configuration[i]->setRotation(rotation);
    }
  }

  void draw_state(const State& state) {
    window.clear(sf::Color(30, 30, 30));
    drawBoard();

    // draw tiles by mapping state.tiles[grid_index] -> tile type
    for (int gridIndex = 1; gridIndex < 10; ++gridIndex) {
      TileNames tileType = state.tiles[gridIndex];
      // compute the top-left position inside the board for this grid cell
      sf::Vector2f pos = get_position(static_cast<int8_t>(gridIndex));
      // place the prototype for that tile type at the computed position
      configuration[static_cast<int>(tileType)]->setPosition(pos);
      window.draw(*configuration[static_cast<int>(tileType)]);
    }
    if (state.pawn_pos != 0) {
      // draw pawn centered in its tile
      sf::Vector2f pawnTilePos = get_position(state.pawn_pos);
      float pawnRadius = 30.f;
      sf::CircleShape pawn(pawnRadius);
      // center the pawn inside the 200x200 tile
      pawn.setPosition(pawnTilePos +
                       sf::Vector2f(100.f - pawnRadius, 100.f - pawnRadius));
      // #rgba(10, 0, 48, 1)
      pawn.setFillColor(sf::Color(10, 0, 30));
      window.draw(pawn);
    }
    window.display();
  }

  sf::RenderWindow& get_window() { return window; }

 private:
  sf::RenderWindow window;
  std::array<std::unique_ptr<pieces::CompositeShape>,
             static_cast<std::size_t>(TileNames::End)>
      configuration;

  const float PIECE_SIZE = 200.f;
  const sf::Vector2f BOARD_ORIGIN = {75.f,
                                     75.f};  // top-left of the 650x650 board
  const sf::Vector2f BOARD_OFFSET = {25.f, 25.f};

  static inline float get_angle(const int8_t orientation) {
    // matches your earlier mapping (1->270, 2->180, 3->0, 4->90)
    switch (orientation) {
      case 1:
        return 90;
      case 2:
        return 180;
      case 3:
        return 0;
      case 4:
        return 270;
      default:
        return 0;
    }
  }

  sf::Vector2f get_position(int8_t pos) const {
    /// only for pos != 0
    int index = static_cast<int>(pos);
    if (index < 0 || index > 9) return BOARD_ORIGIN;

    int cellX, cellY;
    if (index == 0) {
      cellX = 0;
      cellY = 0;
    } else {
      int p = index - 1;
      cellX = (p % 3);
      cellY = (p / 3);
    }

    // pixel positions of top left
    return sf::Vector2f(BOARD_ORIGIN.x + BOARD_OFFSET.x + cellX * PIECE_SIZE,
                        BOARD_ORIGIN.y + BOARD_OFFSET.y + cellY * PIECE_SIZE);
  }

  void drawBoard() {
    // board rectangle (650x650) positioned at BOARD_ORIGIN
    sf::RectangleShape board({650.f, 650.f});
    board.setFillColor(pieces::BoardColor);
    board.setPosition(BOARD_ORIGIN);
    window.draw(board);

    sf::RectangleShape opening({50.f, 175.f});
    opening.setFillColor(pieces::Green);
    opening.setPosition(BOARD_ORIGIN + sf::Vector2f(0.f, 37.5f));
    window.draw(opening);
  }
};