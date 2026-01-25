# Codex Project Notes

## Quick Start
- Configure: `cmake -S . -B build`
- Build: `cmake --build build --parallel`
- Run: `build/bin/SudokuSolver.exe --difficulty easy`
- Test: `ctest --test-dir build`

## Conventions
- Language: C++20, prefer standard library (`std::format`, `std::span`, etc.).
- Keep edits ASCII-only unless the file already uses Unicode.
- Keep comments short and only when logic is non-obvious.

## Architecture Notes
- Use modern C++ solutions.
- Explore programming patterns and logic; this is a learning-focused project.
- Keep the codebase modular and readable, with replaceable building blocks.
- Favor obvious performance wins (reduce copies, avoid unnecessary work) over clever tricks.
- Every feature should include a test.

## Design Principles
- Prefer explicit ownership and value types; avoid hidden global state.
- Keep interfaces small and focused; favor composition over inheritance.
- Separate domain logic from IO/CLI concerns.

## Testing Policy
- Add unit tests for new logic and regression tests for any bug fix.
- Prefer clear, deterministic tests; avoid dependence on randomness.
- Name tests after behavior, not implementation details.

## Performance Checklist
- Pass by reference where appropriate and reserve container capacity when sizes are known.
- Avoid allocations in hot paths and unnecessary copies.

## Documentation
- Document public headers and key invariants in code comments or README notes.

## Build/Runtime Tips
- If changing rendering, verify with a run using `--difficulty` or `--game`.
