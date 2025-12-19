# 🏯 Temple Trap Solver

A command-line and graphical solver for the **Temple Trap** puzzle game.

**Game Link:** [Temple Trap – SmartGames](https://www.smartgames.eu/uk/one-player-games/temple-trap-0)

---

## ⚙️ Compilation

### 🧩 Prerequisites
- A C++ compiler supporting **C++23**
- **CMake**
- **Ninja** (preferred) or **Make**
- **SFML** is used for rendering the board.  
  The library itself is automatically built from source via CMake,  
  but you must install some **system dependencies** (listed below).
  On **Windows** msvc toolchain required for C++ compilation.(Not tested in MinGW)

---

### 📦 Install dependencies for SFML (Linux)

```bash
# Core: window, graphics, and system modules
sudo apt install libx11-dev libxrandr-dev libudev-dev libfreetype-dev libjpeg-dev

# Rendering and window creation (OpenGL + X11)
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libxi-dev libgl1-mesa-dev

# Audio support (sound + music)
sudo apt install libopenal-dev libflac-dev libvorbis-dev

# Input devices
sudo apt install libudev-dev
```

---

### 🏗️ Build the project

```bash
# Configure CMake
cmake -S . -B build --preset=release

# Compile
cmake --build build --preset=Release
```
please don't use the wasm preset, thats not fully prepared yet.

---

### ▶️ Run the executable

The compiled binary will be located in:
```bash
build/release/bin/Release/solver
```
*(Exact path may vary depending on OS and toolchain.)*

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
- If no path exists → prints **“Path not found”**
- If a path exists:
  - Displays the **number of moves**
  - Prints the **sequence of moves**
  - Opens a graphical window (via SFML)  
    where you can navigate states using **Left** and **Right** arrow keys.

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
| [`renderer.hpp`](src/renderer.hpp) | Visualization logic using SFML rectangles. |

### Refer to the **comments** in the source files for detailed documentation.
---

