# Fly 'n' Shoot — Linux / SDL3 port

A working Linux desktop port of Quantum Leaps' **Fly 'n' Shoot** QP/C++ active-object example. SDL3 provides the desktop display and input; QP/C++ runs the original QM-generated game state machines.

**Status (October 2026):** the game runs on Linux. Optional QS instrumentation streams live execution traces to QSpy over TCP.

## Build and play

Prerequisites: Linux, a C++17 compiler, CMake 3.20 or newer, SDL3 development files, and the **QP/C++ 7.3.2 source tree** (not bundled with this repository).

On Debian/Ubuntu, install the general build tools:

```sh
sudo apt update
sudo apt install build-essential cmake pkg-config git
```

Install SDL3 **if your distribution provides it**:

```sh
sudo apt install libsdl3-dev
```

If APT cannot locate `libsdl3-dev`, your distribution may be too old to package SDL3. Build/install SDL3 separately from its upstream source, or use a newer distribution; see [detailed dependency notes](linux-sdl3/README.md#sdl3-dependency).

From the repository root, assuming QP/C++ 7.3.2 is available at `~/qp_old/qpcpp`:

```sh
cmake -S linux-sdl3 -B build/linux-sdl3 \
  -DFLY_BUILD_QP_GAME=ON \
  -DQP_ROOT="$HOME/qp_old/qpcpp"
cmake --build build/linux-sdl3 -j2
./build/linux-sdl3/fly_n_shoot_qp
```

**Controls:** Up arrow — thrust; Space — fire; Escape — quit.

The QP/C++ framework must be obtained separately under its applicable license. Point `QP_ROOT` at the root of its **7.3.2** source tree (containing `ports/posix/qf_port.cpp`). For QS-enabled builds and QSpy instructions, see [linux-sdl3/README.md](linux-sdl3/README.md).

## Architecture and research purpose

The Linux implementation lives in `linux-sdl3/`, which contains the CMake project, SDL3 main loop, platform BSP and framebuffer bridge. The original game code in `qwin-gui/game-gui/` is compiled without editing the QM-generated state machines. The game retains its logical 128×128 display, nominal 33 Hz tick, collision logic and active-object event protocol.

The instrumented executable provides a useful test case for investigating collaboration diagrams of communicating active objects in the separate [collab](https://github.com/smr547/collab) project. QS traces can be compared with the intended model; trace capture, filtering and sequence-diagram experiments remain future work.

## Original source and provenance

The original examples are preserved under `qwin-gui/`:

- `qwin-gui/game-gui/`: Fly 'n' Shoot Windows/QWin GUI example.
- `qwin-gui/dpp-gui/`: Dining Philosophers Windows/QWin GUI example, retained for later experiments.
- `qwin-gui/README.md`: Quantum Leaps' description of dual targeting and QWin.

The example source headers identify **QP/C++ 7.3.0-era APIs** and the generated `game.hpp` identifies **QM 7.0.1**. The verified local build uses **QP/C++ 7.3.2**. Source-level copyright and licensing notices are retained. The exact upstream archive/tag/commit from which the preserved files were copied has **not yet been verified**; see [docs/provenance.md](docs/provenance.md).

For the development plan and historical milestones, see [docs/port-plan.md](docs/port-plan.md).

## License

GPL-3.0-or-later for the Quantum Leaps game example and this port, subject to the individual notices in source files and assets. See [LICENSE](LICENSE). Quantum Leaps retains copyright in its original work. The QWin toolkit is described separately as MIT-licensed in its upstream README; this repository does not currently contain the QWin toolkit implementation.
