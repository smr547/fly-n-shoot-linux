# Linux / SDL3 port plan

## Constraints

- Preserve `game.qm` and all QM-generated game files unchanged for the first working port.
- Preserve the BSP API in `bsp.hpp` wherever feasible.
- Keep original Windows examples under `qwin-gui/` unchanged.
- Match the example's QP/C++ 7.3.0 API before considering framework upgrades.
- Do not replace game physics, framebuffer algorithms or collision detection merely to accommodate SDL3.

## Architecture observations

The game contains three active objects: Tunnel, Ship and Missile. Mine1/Mine2 are subordinate hierarchical state machines synchronously dispatched by Tunnel. The original BSP has a 128×128 logical display and a 33 Hz tick rate. The Win32 implementation uses a bit-packed framebuffer and QWin display widgets.

The Linux adaptation needs an explicit threading contract: SDL window creation, event polling and rendering on the SDL/main thread; QP active objects on the compatible QP POSIX execution environment. Use a synchronized framebuffer snapshot or equivalent ownership handoff. SDL input must be translated to the existing QP event protocol. Confirm exact QP ticker behavior and shutdown ordering before implementation.

## Milestones

1. **Baseline:** confirm upstream provenance, framework version and license notices.
2. **SDL smoke test:** CMake builds an SDL3 window showing a scaled 128×128 test framebuffer; clean window-close behavior.
3. **QP integration:** initialize framework, pools, publish-subscribe and active objects; confirm 33 Hz ticker.
4. **BSP:** implement input, display, collision helpers and shutdown while retaining original model files.
5. **Acceptance:** game starts; ship launches; throttle moves ship; trigger fires missile; tunnel/mines/collisions/scoring/explosion/game-over work; closing window exits cleanly.
6. **Research:** document the platform-dependent delta and the active-object/subordinate-HSM collaboration graph.

## Deferred

- Port the DPP GUI example after the Fly 'n' Shoot implementation works.
- Consider common SDL3 adapter components only after comparing the two ports.
- Treat QP version upgrades and changes to the QM model as separate experiments.

## Laptop prerequisites to check

```sh
cmake --version
pkg-config --modversion sdl3
git status --short --branch
```

No assumption is made that SDL3 or the QP 7.3.0 POSIX port is installed.
