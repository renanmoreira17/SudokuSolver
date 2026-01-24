# SudokuSolver

Small C++20 Sudoku solver with human-style techniques.

Build:

```sh
cmake -S . -B build
cmake --build build
```

Run:

```sh
./build/bin/SudokuSolver --difficulty easy
```

Puzzle input:

```sh
./build/bin/SudokuSolver --game ".7...51.6.6.1....8....8..4.....43...9...614......9..5.5.43...9.7..4..53......8..."
```

Options:
- `--game <puzzle>`: 81-char puzzle string with digits and '.' or '0' for empty.
- `--difficulty <level>`: `any|simple|easy|intermediate|expert` (default: `any`).
- `--report`: print technique explanations as they run.
