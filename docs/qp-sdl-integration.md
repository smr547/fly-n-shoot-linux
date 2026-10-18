# QP/C++ 7.3.2 + SDL3 integration contract

The original QM-generated files in `qwin-gui/game-gui` remain untouched.
The SDL smoke test remains independently buildable.

## Thread ownership

- **Main thread:** SDL_Init, SDL window, renderer, texture, SDL_PollEvent,
  SDL_UpdateTexture, SDL_RenderPresent, and SDL destruction.
- **QP thread(s):** original Tunnel, Ship, Missile active objects and QP
  ticker, using the QP/C++ 7.3.2 POSIX port.
- **Bridge:** BSP::updateScreen() publishes a *snapshot* of the original
  128x128 monochrome bit-packed framebuffer. The SDL main thread alone
  converts snapshots to ARGB8888 pixels and presents them.
- **Input:** SDL keyboard events are translated to the existing
  PLAYER_SHIP_MOVE_SIG, PLAYER_TRIGGER_SIG and PLAYER_QUIT_SIG protocol;
  no new application-level active object is required initially.

## Invariants

1. Do not call SDL rendering APIs from QP active-object threads.
2. Do not let SDL read the mutable BSP framebuffer while QP writes it.
   Use a mutex-protected snapshot or double buffer; keep lock hold times short.
3. Keep the existing game geometry, bitmap data, collision and wall logic
   intact when extracting them from the Win32 BSP.
4. Keep the X11 compositor-bypass hint set to `0` *before* SDL_Init().
   This was experimentally verified to avoid desktop-wide black flashes
   on a three-monitor X11 desktop, including application shutdown.
5. Coordinate shutdown: request QP stop, join its execution thread(s),
   then destroy SDL resources. Do not destroy the renderer while the BSP
   can still publish a frame.
6. Confirm the QP 7.3.2 POSIX port's precise ticker, startup and shutdown
   interfaces against the installed headers before enabling the game target.

## Next implementation slices

1. Extract framebuffer, font, bitmaps, wall and collision routines from
   the Windows BSP into a Linux BSP, preserving original attribution.
2. Implement the snapshot bridge and display it in the SDL main thread.
3. Wire the QP/C++ POSIX port and original game AOs into a separate game
   executable. Keep the smoke executable as a diagnostic.
4. Map keyboard inputs to existing game events and verify graceful exit.
5. Build and test on the Ubuntu machine against its actual QP 7.3.2 tree.

The optional `FLY_BUILD_QP_GAME` CMake switch is intentionally guarded
until the complete source list and POSIX port are verified. It must not
produce a misleading partially linked game.
