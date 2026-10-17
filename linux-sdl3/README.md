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
