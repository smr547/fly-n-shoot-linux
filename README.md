# Fly 'n' Shoot — Linux / SDL3 port

An experimental Linux desktop port of Quantum Leaps' **Fly 'n' Shoot** QP/C++ active-object example, using SDL3 for desktop input and display.

**Status (18 October 2026):** Linux/SDL3 game runs on the target laptop, with the original QM-generated state machines unchanged. Optional QS instrumentation builds successfully and streams live execution traces to QSpy 7.3.1 over TCP. See [Linux build and QS instructions](linux-sdl3/README.md).

## Objectives

1. Run the original game on Linux with SDL3.
2. Keep the original QM model and QM-generated application code unchanged.
3. Preserve the existing BSP contract wherever practical.
4. Document any unavoidable platform coupling, especially event dispatch, timing, rendering and thread ownership.
5. Use the working game as a case study for collaboration diagrams in the separate `collab` project.

## Original source and provenance

The original examples are preserved under `qwin-gui/`:

- `qwin-gui/game-gui/`: Fly 'n' Shoot Windows/QWin GUI example.
- `qwin-gui/dpp-gui/`: Dining Philosophers Windows/QWin GUI example, reserved for a later port.
- `qwin-gui/README.md`: Quantum Leaps' description of dual targeting and QWin.

The example source headers identify **QP/C++ 7.3.0-era APIs** and the generated `game.hpp` identifies **QM 7.0.1**. The established local framework baseline for this experiment is **QP/C++ 7.3.2**, installed at `~/qp_old/qpcpp/`. Source-level copyright and licensing notices are retained. The exact upstream archive/tag/commit from which these files were copied has **not yet been verified**; record it before claiming byte-for-byte upstream identity. See [docs/provenance.md](docs/provenance.md).

## License

GPL-3.0-or-later for the Quantum Leaps game example and this port, subject to the individual notices in source files and assets. See [LICENSE](LICENSE). Quantum Leaps retains copyright in its original work. The QWin toolkit is described separately as MIT-licensed in its upstream README; this repository does not currently contain the QWin toolkit implementation.

## Linux implementation

`linux-sdl3/` contains the CMake project, Linux BSP and SDL3 integration. The original QM-generated game code in `qwin-gui/game-gui/` is built without modifying its state machines.

The port retains the game's logical 128×128 pixel display, nominal 33 Hz tick rate, bitmap/collision logic and active-object event protocol. SDL3 handles window/input/presentation; QP/C++ handles application event processing. Runtime QS traces provide a new basis for investigating active-object collaborations in `collab`.

See [docs/port-plan.md](docs/port-plan.md) for milestones and acceptance criteria.

## Development

Branch: `feature/linux-sdl3-port`.

Build and run instructions, including the separately configured QS-enabled build and QSpy TCP options, are documented in [linux-sdl3/README.md](linux-sdl3/README.md).
