#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/optional.h>
#include "board.hpp"
#include "game_controller.hpp"

namespace nb = nanobind;

NB_MODULE(temple_trap_core, m) {
    nb::enum_<TileNames>(m, "TileNames")
        .value("Goal", TileNames::Goal)
        .value("A", TileNames::A)
        .value("B", TileNames::B)
        .value("C", TileNames::C)
        .value("D", TileNames::D)
        .value("E", TileNames::E)
        .value("F", TileNames::F)
        .value("G", TileNames::G)
        .value("H", TileNames::H)
        .value("Water", TileNames::Water)
        .value("End", TileNames::End);

    nb::class_<State>(m, "State")
        .def(nb::init<>())
        .def_rw("pawn_pos", &State::pawn_pos)
        .def_rw("water_pos", &State::water_pos)
        .def_rw("tiles", &State::tiles)
        .def("is_goal", &State::is_goal);

    nb::class_<GameController>(m, "GameController")
        .def(nb::init<const input_tile_data_t&, int8_t>())
        .def("get_current_state", &GameController::get_current_state)
        .def("is_goal", &GameController::is_goal)
        .def("get_hint", &GameController::get_hint)
        .def("move_tile", &GameController::move_tile)
        .def("move_pawn", &GameController::move_pawn)
        .def("set_state", &GameController::set_state);
}
