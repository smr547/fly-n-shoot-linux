# Source provenance and licensing

## Source inventory

- `qwin-gui/game-gui/game.qm`: original Fly 'n' Shoot QM model.
- `qwin-gui/game-gui/game.hpp`, `ship.cpp`, `missile.cpp`, `mine1.cpp`, `mine2.cpp`, `tunnel.cpp`: generated model/application files.
- `qwin-gui/game-gui/bsp.cpp`, `bsp.hpp`, `main.cpp`: original Win32/QWin target adaptation and startup.
- `qwin-gui/game-gui/Res/`: original GUI bitmap resources.
- `qwin-gui/dpp-gui/`: original Windows DPP GUI example (future experiment).

The game files' headers reference QP/C++ 7.3.0 and dates in 2023. Generated files identify QM 7.0.1 and SPDX-License-Identifier: GPL-3.0-or-later. These are **source-declared versions**, not independently verified upstream provenance.

## Provenance verification still required

1. Identify the precise public Quantum Leaps distribution URL and upstream release/tag/commit used to populate this repository.
2. Compare the imported files against that distribution (e.g. `diff -ru` or `sha256sum`).
3. Record any pre-existing local modifications separately.
4. Inspect individual notices and distribution rights for all `Res/` bitmap/icon assets.
5. Pin the QP/C++ framework dependency to a known compatible public GPL release; do not include evaluation/commercial-only files.

Do not claim that the upstream tree is byte-identical until verification is complete.

## Attribution and derivative work

Preserve all Quantum Leaps notices in copied/modified files. Identify new Linux-specific files and modifications with their own appropriate copyright and GPL-3.0-or-later notices. The repository-level LICENSE contains the GPLv3 text; individual source notices may permit version 3 **or later**.

The upstream `qwin-gui/README.md` describes the separate QWin toolkit as MIT-licensed. Its implementation is not part of this imported tree.
