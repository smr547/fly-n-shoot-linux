# SDL3 smoke test

This is the first Linux build milestone, **not yet the QP game**.

Requires SDL3 development files and CMake. From the repository root:

```sh
cmake -S linux-sdl3 -B build/linux-sdl3 -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-sdl3 -j
./build/linux-sdl3/fly_n_shoot_sdl_smoke
```

Expected: a 512×512 window displaying a 128×128 logical framebuffer, turquoise crosshair and moving yellow rectangle. Press Escape or close the window to exit.

If CMake cannot find SDL3 installed under `/usr/local`, configure with `-DCMAKE_PREFIX_PATH=/usr/local`.

The test uses SDL3's main-thread event loop, an ARGB8888 streaming texture and nearest-neighbour integer scaling. It runs a nominal 33-frame/s pacing loop; actual frame timing is not yet measured. QP integration, original bit-packed framebuffer and input mapping are next.


## Experimental QP/C++ game integration (first build)

The original QM-generated game sources are linked without editing them. Supply the
local QP/C++ 7.3.2 source tree; it is not copied into this repository.

```sh
git switch feature/linux-sdl3-port
git pull --ff-only
cmake -S linux-sdl3 -B build/linux-sdl3 \
  -DFLY_BUILD_QP_GAME=ON \
  -DQP_ROOT="$HOME/qp_old/qpcpp"
cmake --build build/linux-sdl3 -j2
./build/linux-sdl3/fly_n_shoot_qp
```

Keys: Space fires, Up arrow is throttle, Escape closes the window.
The X11 compositor-bypass hint remains disabled.

**Status:** first integration build; not yet verified on the target laptop.
The QP POSIX port spawns detached active-object threads, so shutdown behavior
also needs target testing. Report the first compiler error verbatim if any.
The independent `fly_n_shoot_sdl_smoke` target remains available.


## QS tracing over TCP (experimental)

The optional `FLY_ENABLE_QS` build uses the **unmodified QP/C++ 7.3.2
POSIX QS TCP port** (`ports/posix/qs_port.cpp`). It does not send binary
trace data to stdout. QS is off by default, preserving the known-working
non-tracing game.

Start the QSpy TCP server in a separate terminal **before launching the game**:

```sh
qspy -t
```

Then build and launch the instrumented game:

```sh
cmake -S linux-sdl3 -B build/linux-sdl3-qs \
  -DFLY_BUILD_QP_GAME=ON -DFLY_ENABLE_QS=ON \
  -DQP_ROOT="$HOME/qp_old/qpcpp"
cmake --build build/linux-sdl3-qs -j2
./build/linux-sdl3-qs/fly_n_shoot_qp
```

The default target is `localhost:6601`. Override with
`FLY_QSPY_TARGET=hostname:port` if necessary. A failed QS connection
terminates the instrumented game rather than silently disabling tracing.
The code installs AO and selected signal dictionaries and outputs QS
records on each QP clock tick. For high event rates, adjust QS filters
to control trace volume. This build awaits testing against the user's
local QSpy executable; command-line flags may differ between versions.
