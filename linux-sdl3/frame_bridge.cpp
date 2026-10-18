// SPDX-License-Identifier: GPL-3.0-or-later
#include "frame_bridge.hpp"
#include <algorithm>
#include <mutex>
namespace {
std::mutex mutex;
FrameBridge::Snapshot latest{};
}
namespace FrameBridge {
void publish(std::uint32_t const *words, unsigned stride_words, unsigned rows) {
    std::lock_guard<std::mutex> lock(mutex);
    for (unsigned y=0; y<std::min(rows,HEIGHT); ++y) {
        std::copy_n(words+y*stride_words,WIDTH/32U,latest.data()+y*(WIDTH/32U));
    }
}
Snapshot snapshot() {
    std::lock_guard<std::mutex> lock(mutex);
    return latest;
}
}
