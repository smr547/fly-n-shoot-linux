// SPDX-License-Identifier: GPL-3.0-or-later
// SDL3 smoke test for the Linux Fly 'n' Shoot port.
// This file is not part of the original Quantum Leaps model.
#include <SDL3/SDL.h>

#include <array>
#include <cstdint>
#include <cstdio>

namespace {
constexpr int WIDTH = 128;
constexpr int HEIGHT = 128;
constexpr int SCALE = 4;
constexpr int TARGET_FPS = 33;
using Framebuffer = std::array<std::uint32_t, WIDTH * HEIGHT>;

void draw_test_pattern(Framebuffer &pixels, std::uint64_t frame) {
    constexpr std::uint32_t background = 0xFF101820U;
    constexpr std::uint32_t foreground = 0xFF40E0D0U;
    constexpr std::uint32_t accent = 0xFFFFC857U;
    pixels.fill(background);
    for (int x = 0; x < WIDTH; ++x) {
        pixels[static_cast<std::size_t>(HEIGHT / 2) * WIDTH + x] = foreground;
    }
    for (int y = 0; y < HEIGHT; ++y) {
        pixels[static_cast<std::size_t>(y) * WIDTH + WIDTH / 2] = foreground;
    }
    const int x = static_cast<int>(frame % WIDTH);
    for (int y = 20; y < 36; ++y) {
        for (int dx = 0; dx < 12; ++dx) {
            pixels[static_cast<std::size_t>(y) * WIDTH + ((x + dx) % WIDTH)] = accent;
        }
    }
}
}

int main(int, char **) {
    // Avoid desktop-wide compositor transitions on multi-monitor X11 setups.
    // Set before SDL_Init(); SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR=0
    // was confirmed to prevent black flashes on window startup.
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow(
        "Fly 'n' Shoot - SDL3 smoke test", WIDTH * SCALE, HEIGHT * SCALE, 0);
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                            SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (!texture) {
        std::fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(renderer, WIDTH, HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    Framebuffer pixels{};
    std::uint64_t frame = 0;
    bool running = true;
    constexpr std::uint64_t frame_ns = 1000000000ULL / TARGET_FPS;
    std::uint64_t next_frame = SDL_GetTicksNS();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT ||
                (event.type == SDL_EVENT_KEY_DOWN &&
                 event.key.key == SDLK_ESCAPE)) {
                running = false;
            }
        }
        if (!running) {
            break;
        }
        draw_test_pattern(pixels, frame++);
        if (!SDL_UpdateTexture(texture, nullptr, pixels.data(),
                               WIDTH * static_cast<int>(sizeof(std::uint32_t)))) {
            std::fprintf(stderr, "SDL_UpdateTexture failed: %s\n", SDL_GetError());
            break;
        }
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        next_frame += frame_ns;
        const std::uint64_t now = SDL_GetTicksNS();
        if (now < next_frame) {
            SDL_DelayNS(next_frame - now);
        } else if (now - next_frame > frame_ns) {
            next_frame = now;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
