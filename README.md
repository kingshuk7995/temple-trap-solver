# 🏯 Temple Trap Solver

A command-line and graphical solver for the **Temple Trap** puzzle game.

**Game Link:** [Temple Trap – SmartGames](https://www.smartgames.eu/uk/one-player-games/temple-trap-0)

---

## ⚙️ Compilation

### 🧩 Prerequisites
- A C++ compiler supporting **C++23**
- **CMake**
- **Ninja** (preferred) or **Make**
- **Make** for compilation

---

### 🏗️ Build the project

```bash
make
```

---

### ▶️ Run the executable

The compiled binary will be located in:
```bash
make run
```

You can run it directly from the terminal.

---

## 🎮 Running the Solver

- The solver takes input interactively from the terminal.  
- For each tile **A to H**, provide:
  - **Position** on the board (1–9)
  - **Orientation** (1–4)
- Provide the **Pawn** position on the board (1-9)
- The solver then applies the **A\*** algorithm to find a solution.

### 🧾 Output:
  - Opens a gui where you can play and get next move hint.
  - Moves are cached.

---

## 🧠 Approach

- The solver uses an **A\*** search algorithm with a custom heuristic.
- The heuristic is a **modified Manhattan distance** between the pawn and the goal.
- Implementation involves two main classes:
  - **`Board`** → Encapsulates tile orientations and board structure.
  - **`State`** → Represents the full configuration at each search step.

---

## 🧩 Implementation Details

| File | Description |
|------|--------------|
| [`solver.hpp`](src/solver.hpp) | Type-safe, generic A\* algorithm implementation. |
| [`board.hpp`](src/board.hpp) | Implements the `Board` and `State` classes. |

### Refer to the **comments** in the source files for detailed documentation.
---

