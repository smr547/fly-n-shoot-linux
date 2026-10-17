# Source provenance and licensing

## Source inventory

- `qwin-gui/game-gui/game.qm`: original Fly 'n' Shoot QM model.
- `qwin-gui/game-gui/game.hpp`, `ship.cpp`, `missile.cpp`, `mine1.cpp`, `mine2.cpp`, `tunnel.cpp`: generated model/application files.
- `qwin-gui/game-gui/bsp.cpp`, `bsp.hpp`, `main.cpp`: original Win32/QWin target adaptation and startup.
- `qwin-gui/game-gui/Res/`: original GUI bitmap resources.
- `qwin-gui/dpp-gui/`: original Windows DPP GUI example (future experiment).

The game files' headers reference QP/C++ 7.3.0 and dates in 2023. The local archive `~/qp_old/qpcpp/include/qp.hpp` was previously inspected and identifies QP/C++ 7.3.2. Previous DPP work identified the official `qpcpp_7.3.2.zip` distribution and verified a DPP model against that archive using `cmp`. The game source was copied from `~/qp_old/qp/qpcpp/examples/qwin-gui/`; the framework tree also contains `~/qp_old/qpcpp/examples/qwin-gui/`. Generated files identify QM 7.0.1 and SPDX-License-Identifier: GPL-3.0-or-later. These are **source-declared versions**, not independently verified upstream provenance.

## Provenance verification still required

1. Record the previously identified official Quantum Leaps `qpcpp_7.3.2.zip` archive as the framework provenance, and compare the game files against the user's archived `~/qp_old/qp/qpcpp/examples/qwin-gui/` source. Do not assume the separate `qp/` copy and `qpcpp/` copy are identical without checking.
2. Compare the imported files against that distribution (e.g. `diff -ru` or `sha256sum`).
3. Record any pre-existing local modifications separately.
4. Inspect individual notices and distribution rights for all `Res/` bitmap/icon assets.
5. Build against the existing local QP/C++ 7.3.2 GPL installation at `~/qp_old/qpcpp/`; do not include evaluation/commercial-only files.

Do not claim that the upstream tree is byte-identical until verification is complete.

## Attribution and derivative work

Preserve all Quantum Leaps notices in copied/modified files. Identify new Linux-specific files and modifications with their own appropriate copyright and GPL-3.0-or-later notices. The repository-level LICENSE contains the GPLv3 text; individual source notices may permit version 3 **or later**.

The upstream `qwin-gui/README.md` describes the separate QWin toolkit as MIT-licensed. Its implementation is not part of this imported tree.
