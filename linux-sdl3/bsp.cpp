// Linux SDL3 BSP derived from Quantum Leaps Fly 'n' Shoot Win32 BSP.
// Original copyright and GPL-3.0-or-later license retained.
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"
#include "frame_bridge.hpp"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
Q_DEFINE_THIS_FILE
static uint32_t l_fb[BSP::SCREEN_HEIGHT + 1][BSP::SCREEN_WIDTH / 32U] {};
static uint32_t l_walls[GAME_TUNNEL_HEIGHT + 1][BSP::SCREEN_WIDTH / 32U] {};
static unsigned l_rnd = 1234U;
static std::atomic<bool> l_throttle {false};
static void paintBits(uint8_t x, uint8_t y, uint8_t const *bits, uint8_t h);
static void paintBitsClear(uint8_t x, uint8_t y, uint8_t const *bits, uint8_t h);
}

extern "C" Q_NORETURN Q_onError(char const * const module, int_t const loc) {
    std::fprintf(stderr, "QP assertion: %s:%d\n", module, loc);
    std::abort();
}

namespace BSP {
void init() { randomSeed(1234U); }
void terminate(std::int16_t) { QP::QF::stop(); }
void updateScreen() {
    FrameBridge::publish(&l_fb[0][0], SCREEN_WIDTH / 32U, SCREEN_HEIGHT);
}
void clearFB() {
    for (unsigned y=0; y<SCREEN_HEIGHT; ++y) {
        for (unsigned x=0; x<SCREEN_WIDTH/32U; ++x) l_fb[y][x]=0U;
    }
}
void clearWalls() {
    for (unsigned y=0; y<GAME_TUNNEL_HEIGHT; ++y) {
        for (unsigned x=0; x<SCREEN_WIDTH/32U; ++x) l_walls[y][x]=0U;
    }
}
bool isThrottle() { return l_throttle.load(std::memory_order_relaxed); }
void setThrottle(bool value) { l_throttle.store(value, std::memory_order_relaxed); }
void paintString(uint8_t x, uint8_t y, char const *str) {
    static uint8_t const font5x7[95][7] = {
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U }, //
        { 0x04U, 0x04U, 0x04U, 0x04U, 0x00U, 0x00U, 0x04U }, // !
        { 0x0AU, 0x0AU, 0x0AU, 0x00U, 0x00U, 0x00U, 0x00U }, // "
        { 0x0AU, 0x0AU, 0x1FU, 0x0AU, 0x1FU, 0x0AU, 0x0AU }, // #
        { 0x04U, 0x1EU, 0x05U, 0x0EU, 0x14U, 0x0FU, 0x04U }, // $
        { 0x03U, 0x13U, 0x08U, 0x04U, 0x02U, 0x19U, 0x18U }, // %
        { 0x06U, 0x09U, 0x05U, 0x02U, 0x15U, 0x09U, 0x16U }, // &
        { 0x06U, 0x04U, 0x02U, 0x00U, 0x00U, 0x00U, 0x00U }, // '
        { 0x08U, 0x04U, 0x02U, 0x02U, 0x02U, 0x04U, 0x08U }, // (
        { 0x02U, 0x04U, 0x08U, 0x08U, 0x08U, 0x04U, 0x02U }, // )
        { 0x00U, 0x04U, 0x15U, 0x0EU, 0x15U, 0x04U, 0x00U }, // *
        { 0x00U, 0x04U, 0x04U, 0x1FU, 0x04U, 0x04U, 0x00U }, // +
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x04U, 0x02U }, // ,
        { 0x00U, 0x00U, 0x00U, 0x1FU, 0x00U, 0x00U, 0x00U }, // -
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x06U, 0x06U }, // .
        { 0x00U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x00U }, // /
        { 0x0EU, 0x11U, 0x19U, 0x15U, 0x13U, 0x11U, 0x0EU }, // 0
        { 0x04U, 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, // 1
        { 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x02U, 0x1FU }, // 2
        { 0x1FU, 0x08U, 0x04U, 0x08U, 0x10U, 0x11U, 0x0EU }, // 3
        { 0x08U, 0x0CU, 0x0AU, 0x09U, 0x1FU, 0x08U, 0x08U }, // 4
        { 0x1FU, 0x01U, 0x0FU, 0x10U, 0x10U, 0x11U, 0x0EU }, // 5
        { 0x0CU, 0x02U, 0x01U, 0x0FU, 0x11U, 0x11U, 0x0EU }, // 6
        { 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x02U, 0x02U }, // 7
        { 0x0EU, 0x11U, 0x11U, 0x0EU, 0x11U, 0x11U, 0x0EU }, // 8
        { 0x0EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x08U, 0x06U }, // 9
        { 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x06U, 0x00U }, // :
        { 0x00U, 0x06U, 0x06U, 0x00U, 0x06U, 0x04U, 0x02U }, // ;
        { 0x08U, 0x04U, 0x02U, 0x01U, 0x02U, 0x04U, 0x08U }, // <
        { 0x00U, 0x00U, 0x1FU, 0x00U, 0x1FU, 0x00U, 0x00U }, // =
        { 0x02U, 0x04U, 0x08U, 0x10U, 0x08U, 0x04U, 0x02U }, // >
        { 0x0EU, 0x11U, 0x10U, 0x08U, 0x04U, 0x00U, 0x04U }, // ?
        { 0x0EU, 0x11U, 0x10U, 0x16U, 0x15U, 0x15U, 0x0EU }, // @
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U }, // A
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x11U, 0x11U, 0x0FU }, // B
        { 0x0EU, 0x11U, 0x01U, 0x01U, 0x01U, 0x11U, 0x0EU }, // C
        { 0x07U, 0x09U, 0x11U, 0x11U, 0x11U, 0x09U, 0x07U }, // D
        { 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x1FU }, // E
        { 0x1FU, 0x01U, 0x01U, 0x0FU, 0x01U, 0x01U, 0x01U }, // F
        { 0x0EU, 0x11U, 0x01U, 0x1DU, 0x11U, 0x11U, 0x1EU }, // G
        { 0x11U, 0x11U, 0x11U, 0x1FU, 0x11U, 0x11U, 0x11U }, // H
        { 0x0EU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, // I
        { 0x1CU, 0x08U, 0x08U, 0x08U, 0x08U, 0x09U, 0x06U }, // J
        { 0x11U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U, 0x11U }, // K
        { 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x1FU }, // L
        { 0x11U, 0x1BU, 0x15U, 0x15U, 0x11U, 0x11U, 0x11U }, // M
        { 0x11U, 0x11U, 0x13U, 0x15U, 0x19U, 0x11U, 0x11U }, // N
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU }, // O
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x01U, 0x01U, 0x01U }, // P
        { 0x0EU, 0x11U, 0x11U, 0x11U, 0x15U, 0x09U, 0x16U }, // Q
        { 0x0FU, 0x11U, 0x11U, 0x0FU, 0x05U, 0x09U, 0x11U }, // R
        { 0x1EU, 0x01U, 0x01U, 0x0EU, 0x10U, 0x10U, 0x0FU }, // S
        { 0x1FU, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U }, // T
        { 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0EU }, // U
        { 0x11U, 0x11U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U }, // V
        { 0x11U, 0x11U, 0x11U, 0x15U, 0x15U, 0x15U, 0x0AU }, // W
        { 0x11U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U, 0x11U }, // X
        { 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U, 0x04U, 0x04U }, // Y
        { 0x1FU, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U, 0x1FU }, // Z
        { 0x0EU, 0x02U, 0x02U, 0x02U, 0x02U, 0x02U, 0x0EU }, // [
        { 0x00U, 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x00U }, // '\'
        { 0x0EU, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x0EU }, // ]
        { 0x04U, 0x0AU, 0x11U, 0x00U, 0x00U, 0x00U, 0x00U }, // ^
        { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x1FU }, // _
        { 0x02U, 0x04U, 0x08U, 0x00U, 0x00U, 0x00U, 0x00U }, // `
        { 0x00U, 0x00U, 0x0EU, 0x10U, 0x1EU, 0x11U, 0x1EU }, // a
        { 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x0FU }, // b
        { 0x00U, 0x00U, 0x0EU, 0x01U, 0x01U, 0x11U, 0x0EU }, // c
        { 0x10U, 0x10U, 0x16U, 0x19U, 0x11U, 0x11U, 0x1EU }, // d
        { 0x00U, 0x00U, 0x0EU, 0x11U, 0x1FU, 0x01U, 0x0EU }, // e
        { 0x0CU, 0x12U, 0x02U, 0x07U, 0x02U, 0x02U, 0x02U }, // f
        { 0x00U, 0x1EU, 0x11U, 0x11U, 0x1EU, 0x10U, 0x0EU }, // g
        { 0x01U, 0x01U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U }, // h
        { 0x04U, 0x00U, 0x06U, 0x04U, 0x04U, 0x04U, 0x0EU }, // i
        { 0x08U, 0x00U, 0x0CU, 0x08U, 0x08U, 0x09U, 0x06U }, // j
        { 0x01U, 0x01U, 0x09U, 0x05U, 0x03U, 0x05U, 0x09U }, // k
        { 0x06U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x0EU }, // l
        { 0x00U, 0x00U, 0x0BU, 0x15U, 0x15U, 0x11U, 0x11U }, // m
        { 0x00U, 0x00U, 0x0DU, 0x13U, 0x11U, 0x11U, 0x11U }, // n
        { 0x00U, 0x00U, 0x0EU, 0x11U, 0x11U, 0x11U, 0x0EU }, // o
        { 0x00U, 0x00U, 0x0FU, 0x11U, 0x0FU, 0x01U, 0x01U }, // p
        { 0x00U, 0x00U, 0x16U, 0x19U, 0x1EU, 0x10U, 0x10U }, // q
        { 0x00U, 0x00U, 0x0DU, 0x13U, 0x01U, 0x01U, 0x01U }, // r
        { 0x00U, 0x00U, 0x0EU, 0x01U, 0x0EU, 0x10U, 0x0FU }, // s
        { 0x02U, 0x02U, 0x07U, 0x02U, 0x02U, 0x12U, 0x0CU }, // t
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x19U, 0x16U }, // u
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x11U, 0x0AU, 0x04U }, // v
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x15U, 0x15U, 0x0AU }, // w
        { 0x00U, 0x00U, 0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U }, // x
        { 0x00U, 0x00U, 0x11U, 0x11U, 0x1EU, 0x10U, 0x0EU }, // y
        { 0x00U, 0x00U, 0x1FU, 0x08U, 0x04U, 0x02U, 0x1FU }, // z
        { 0x08U, 0x04U, 0x04U, 0x02U, 0x04U, 0x04U, 0x08U }, // {
        { 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U }, // |
        { 0x02U, 0x04U, 0x04U, 0x08U, 0x04U, 0x04U, 0x02U }, // }
        { 0x02U, 0x15U, 0x08U, 0x00U, 0x00U, 0x00U, 0x00U }, // ~
    };

    for (; *str != '\0'; ++str, x += 6) {
        uint8_t const *ch = &font5x7[*str - ' '][0];
        paintBitsClear(x, y, ch, 7);
    }
}


//============================================================================
typedef struct { // the auxiliary structure to hold const bitmaps
    uint8_t const *bits; // the bits in the bitmap
    uint8_t height;      // the height of the bitmap
} Bitmap;

// bitmap of the Ship:
//
//     x....
//     xxx..
//     xxxxx
//
static uint8_t const ship_bits[] = {
    0x01U, 0x07U, 0x1FU
};

// bitmap of the Missile:
//
//     xxxx
//
static uint8_t const missile_bits[] = {
    0x0FU
};

// bitmap of the Mine type-1:
//
//     .x.
//     xxx
//     .x.
//
static uint8_t const mine1_bits[] = {
    0x02U, 0x07U, 0x02U
};

// bitmap of the Mine type-2:
//
//     x..x
//     .xx.
//     .xx.
//     x..x
//
static uint8_t const mine2_bits[] = {
    0x09U, 0x06U, 0x06U, 0x09U
};

// Mine type-2 is nastier than Mine type-1. The type-2 mine can
// hit the Ship with any of its "tentacles". However, it can be
// destroyed by the Missile only by hitting its center, defined as
// the following bitmap:
//
//     ....
//     .xx.
//     .xx.
//
static uint8_t const mine2_missile_bits[] = {
    0x00U, 0x06U, 0x06U
};

//
// The bitmap of the explosion stage 0:
//
//     .......
//     ...x...
//     ..x.x..
//     ...x...
//
static uint8_t const explosion0_bits[] = {
    0x00U, 0x08U, 0x14U, 0x08U
};

//
// The bitmap of the explosion stage 1:
//
//     .......
//     ..x.x..
//     ...x...
//     ..x.x..
//
static uint8_t const explosion1_bits[] = {
    0x00U, 0x14U, 0x08U, 0x14U
};

//
// The bitmap of the explosion stage 2:
//
//     .x...x.
//     ..x.x..
//     ...x...
//     ..x.x..
//     .x...x.
//
static uint8_t const explosion2_bits[] = {
    0x11U, 0x0AU, 0x04U, 0x0AU, 0x11U
};

//
// The bitmap of the explosion stage 3:
//
//     x..x..x
//     .x.x.x.
//     ..x.x..
//     xx.x.xx
//     ..x.x..
//     .x.x.x.
//     x..x..x
//
static uint8_t const explosion3_bits[] = {
    0x49, 0x2A, 0x14, 0x6B, 0x14, 0x2A, 0x49
};

static Bitmap const l_bitmap[GAME::MAX_BMP] = {
    { ship_bits,       Q_DIM(ship_bits) },
    { missile_bits,    Q_DIM(missile_bits) },
    { mine1_bits,      Q_DIM(mine1_bits) },
    { mine2_bits,      Q_DIM(mine2_bits) },
    { mine2_missile_bits, Q_DIM(mine2_missile_bits) },
    { explosion0_bits, Q_DIM(explosion0_bits) },
    { explosion1_bits, Q_DIM(explosion1_bits) },
    { explosion2_bits, Q_DIM(explosion2_bits) },
    { explosion3_bits, Q_DIM(explosion3_bits) }
};

//............................................................................
void paintBitmap(uint8_t x, uint8_t y, uint8_t bmp_id) {
    Bitmap const *bmp = &l_bitmap[bmp_id];
    paintBits(x, y, bmp->bits, bmp->height);
}
//............................................................................
void advanceWalls(uint8_t top, uint8_t bottom) {
    uint_fast8_t y;
    for (y = 0U; y < GAME_TUNNEL_HEIGHT; ++y) {
        // shift the walls one pixel to the left
        l_walls[y][0] = (l_walls[y][0] >> 1) | (l_walls[y][1] << 31);
        l_walls[y][1] = (l_walls[y][1] >> 1) | (l_walls[y][2] << 31);
        l_walls[y][2] = (l_walls[y][2] >> 1) | (l_walls[y][3] << 31);
        l_walls[y][3] = (l_walls[y][3] >> 1);

        // add new column of walls at the end
        if (y <= top) {
            l_walls[y][3] |= (1U << 31);
        }
        if (y >= (GAME_TUNNEL_HEIGHT - bottom)) {
            l_walls[y][3] |= (1U << 31);
        }

        // copy the walls to the frame buffer
        l_fb[y][0] = l_walls[y][0];
        l_fb[y][1] = l_walls[y][1];
        l_fb[y][2] = l_walls[y][2];
        l_fb[y][3] = l_walls[y][3];
    }
}
//............................................................................
bool doBitmapsOverlap(uint8_t bmp_id1, uint8_t x1, uint8_t y1,
                          uint8_t bmp_id2, uint8_t x2, uint8_t y2)
{
    uint8_t y;
    uint8_t y0;
    uint8_t h;
    uint32_t bits1;
    uint32_t bits2;
    Bitmap const *bmp1;
    Bitmap const *bmp2;

    Q_REQUIRE((bmp_id1 < Q_DIM(l_bitmap)) && (bmp_id2 < Q_DIM(l_bitmap)));

    // are the bitmaps close enough in x?
    if (x1 >= x2) {
        if (x1 > x2 + 8U) {
            return false;
        }
        x1 -= x2;
        x2 = 0U;
    }
    else {
        if (x2 > x1 + 8U) {
            return false;
        }
        x2 -= x1;
        x1 = 0U;
    }

    bmp1 = &l_bitmap[bmp_id1];
    bmp2 = &l_bitmap[bmp_id2];
    if ((y1 <= y2) && (y1 + bmp1->height > y2)) {
        y0 = y2 - y1;
        h = y1 + bmp1->height - y2;
        if (h > bmp2->height) {
            h = bmp2->height;
        }
        for (y = 0; y < h; ++y) { // scan over the overlapping rows
            bits1 = ((uint32_t)bmp1->bits[y + y0] << x1);
            bits2 = ((uint32_t)bmp2->bits[y] << x2);
            if ((bits1 & bits2) != 0U) { // do the bits overlap?
                return true; // yes!
            }
        }
    }
    else {
        if ((y1 > y2) && (y2 + bmp2->height > y1)) {
            y0 = y1 - y2;
            h = y2 + bmp2->height - y1;
            if (h > bmp1->height) {
                h = bmp1->height;
            }
            for (y = 0; y < h; ++y) {  // scan over the overlapping rows
                bits1 = ((uint32_t)bmp1->bits[y] << x1);
                bits2 = ((uint32_t)bmp2->bits[y + y0] << x2);
                if ((bits1 & bits2) != 0U) { // do the bits overlap?
                                       return true; // yes!
                }
            }
        }
    }
    return false; // the bitmaps do not overlap
}
//............................................................................
bool isWallHit(uint8_t bmp_id, uint8_t x, uint8_t y) {
    Bitmap const *bmp = &l_bitmap[bmp_id];
    uint32_t shift = (x & 0x1FU);
    uint32_t *walls = &l_walls[y][x >> 5];
    for (y = 0; y < bmp->height; ++y, walls += (SCREEN_WIDTH >> 5)) {
        if (*walls & ((uint32_t)bmp->bits[y] << shift)) {
            return true;
        }
        if (shift > 24U) {
            if (*(walls + 1) & ((uint32_t)bmp->bits[y] >> (32U - shift))) {
                return true;
            }
        }
    }
    return false;
}


void updateScore(uint16_t score) {
    uint8_t seg[5];
    char str[5];

    if (score == 0U) {
        paintString(1U, SCREEN_HEIGHT - 8U, "SCORE:");
    }

    seg[0] = score % 10U; score /= 10U;
    seg[1] = score % 10U; score /= 10U;
    seg[2] = score % 10U; score /= 10U;
    seg[3] = score % 10U;

    // update the SCORE area on the screeen
    str[0] = seg[3] + '0';
    str[1] = seg[2] + '0';
    str[2] = seg[1] + '0';
    str[3] = seg[0] + '0';
    str[4] = '\0';
    paintString(6U*6U, SCREEN_HEIGHT - 8U, str);


}

void displayOn() {}
void displayOff() { clearFB(); updateScreen(); }
uint32_t random(void) {  // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    l_rnd = l_rnd * (3U*7U*11U*13U*23U);
    return l_rnd >> 8;
}
//............................................................................
void randomSeed(std::uint32_t seed) {
    l_rnd = seed;
}


} // namespace BSP

namespace {
static void paintBits(uint8_t x, uint8_t y, uint8_t const *bits, uint8_t h) {
    uint32_t *fb = &l_fb[y][x >> 5];
    uint32_t shift = (x & 0x1FU);
    for (y = 0; y < h; ++y, fb += (BSP::SCREEN_WIDTH >> 5)) {
        *fb |= ((uint32_t)bits[y] << shift);
        if (shift > 24U) {
            *(fb + 1) |= ((uint32_t)bits[y] >> (32U - shift));
        }
    }
}
//............................................................................
static void paintBitsClear(uint8_t x, uint8_t y,
                           uint8_t const *bits, uint8_t h)
{
    uint32_t *fb = &l_fb[y][x >> 5];
    uint32_t shift = (x & 0x1FU);
    uint32_t mask1 = ~((uint32_t)0xFFU << shift);
    uint32_t mask2;
    if (shift > 24U) {
        mask2 = ~(0xFFU >> (32U - shift));
    }
    for (y = 0; y < h; ++y, fb += (BSP::SCREEN_WIDTH >> 5)) {
        *fb = ((*fb & mask1) | ((uint32_t)bits[y] << shift));
        if (shift > 24U) {
            *(fb + 1) = ((*(fb + 1) & mask2)
                | ((uint32_t)bits[y] >> (32U - shift)));
        }
    }
}


} // namespace
