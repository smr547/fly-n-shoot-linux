// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <array>
#include <cstdint>
namespace FrameBridge {
constexpr unsigned WIDTH=128U, HEIGHT=128U;
using Snapshot=std::array<std::uint32_t, WIDTH*HEIGHT/32U>;
void publish(std::uint32_t const *words, unsigned stride_words, unsigned rows);
Snapshot snapshot();
}
