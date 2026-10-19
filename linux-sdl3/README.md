# Linux / SDL3 build notes

The **Fly 'n' Shoot game is running on Linux** with SDL3 and QP/C++ 7.3.2. Run all commands from the repository root.

## SDL3 dependency

Install general build prerequisites on Debian/Ubuntu:

```sh
sudo apt update
sudo apt install build-essential cmake pkg-config git
```

If available for your distribution release, install SDL3 development files:

```sh
sudo apt install libsdl3-dev
```

**Important:** `libsdl3-dev` is not available in every Ubuntu/Debian release. If the package is unavailable, build and install SDL3 from the official [SDL source repository](https://github.com/libsdl-org/SDL), following its Linux build instructions. One typical CMake workflow, assuming suitable build dependencies are installed, is:

```sh
git clone https://github.com/libsdl-org/SDL.git
cmake -S SDL -B SDL/build -DCMAKE_BUILD_TYPE=Release
cmake --build SDL/build -j2
sudo cmake --install SDL/build
sudo ldconfig
```

For reproducibility, choose and record a stable SDL3 release/tag rather than relying on the repository's changing default branch. SDL's own build may require additional development libraries depending on distribution and selected backends.

The project uses `find_package(SDL3 CONFIG REQUIRED)`. If SDL3 is installed somewhere other than the usual system paths, add `-DCMAKE_PREFIX_PATH=/path/to/sdl3/prefix` to the game's CMake configuration. An installation under `/usr/local` may require `-DCMAKE_PREFIX_PATH=/usr/local`.

## QP/C++ dependency

Supply a **QP/C++ 7.3.2 source tree** separately; it is not vendored in this repository. Set `QP_ROOT` to its root, containing `ports/posix/qf_port.cpp`. The verified developer layout is `$HOME/qp_old/qpcpp`.

## Build and run the game

```sh
cmake -S linux-sdl3 -B build/linux-sdl3 \
  -DFLY_BUILD_QP_GAME=ON \
  -DQP_ROOT="$HOME/qp_old/qpcpp"
cmake --build build/linux-sdl3 -j2
./build/linux-sdl3/fly_n_shoot_qp
```

**Controls:** Up arrow — thrust; Space — fire; Escape — quit.

The original QM-generated game sources are compiled without modifications. The SDL3 implementation retains the logical 128×128 framebuffer and nominal 33 Hz update rate. The Linux QP POSIX port uses active-object threads; orderly shutdown is still worth reviewing.

## Optional: QS tracing to QSpy over TCP

The QS build uses the unmodified QP/C++ 7.3.2 POSIX QS TCP port (`ports/posix/qs_port.cpp`). Tracing is **off** in the ordinary game build.

Start QSpy **first**, in a separate terminal. This command was verified with QSpy 7.3.1 on 64-bit Linux:

```sh
/home/smr/qp_old/qtools/bin/qspy -t 6601 -u 0 -v 7.3 -O 8 -F 8
```

Adjust the QSpy executable path for your installation. The options select TCP port 6601, disable the unused UDP socket, select QS protocol 7.3 compatibility and set 64-bit object/function pointers.

Configure and launch the instrumented game in another terminal:

```sh
cmake -S linux-sdl3 -B build/linux-sdl3-qs \
  -DFLY_BUILD_QP_GAME=ON -DFLY_ENABLE_QS=ON \
  -DQP_ROOT="$HOME/qp_old/qpcpp"
cmake --build build/linux-sdl3-qs -j2
./build/linux-sdl3-qs/fly_n_shoot_qp
```

By default, the game connects to `localhost:6601`; `FLY_QSPY_TARGET=hostname:port` overrides this. A failed QS connection terminates the instrumented executable rather than silently disabling tracing.

**Verified October 2026:** QSpy receives and decodes live records, including active-object/state-machine activity. Trace volume is high. QS filtering, controlled binary capture (`qspy -s`) and QSpy sequence-diagram output (`-g`) are promising next experiments for [collab](https://github.com/smr547/collab); those experiments have not yet been completed.

## SDL3-only smoke test

The separate smoke-test executable does not require QP/C++:

```sh
cmake -S linux-sdl3 -B build/linux-sdl3-smoke -DCMAKE_BUILD_TYPE=Debug
cmake --build build/linux-sdl3-smoke -j2
./build/linux-sdl3-smoke/fly_n_shoot_sdl_smoke
```

It opens a 512×512 window showing a scaled 128×128 logical framebuffer, a turquoise crosshair and a moving yellow rectangle. Press Escape or close the window to exit. It uses an ARGB8888 streaming texture and nearest-neighbour scaling.
