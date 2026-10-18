// SPDX-License-Identifier: GPL-3.0-or-later
// Linux entry point for the unmodified Quantum Leaps Fly 'n' Shoot model.
#include <SDL3/SDL.h>
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"
#include "frame_bridge.hpp"
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <cstdlib>

namespace {
constexpr int SCALE=4;
std::atomic<bool> running{true};
std::atomic<bool> qpReady{false};
static QP::QEvt const tickEvt(GAME::TIME_TICK_SIG);
static QP::QEvt const triggerEvt(GAME::PLAYER_TRIGGER_SIG);
static QP::QEvt const quitEvt(GAME::PLAYER_QUIT_SIG);
void publish(QP::QEvt const *event) { QP::QF::PUBLISH(event, nullptr); }

void qpThread() {
    QP::QF::init();
    BSP::init();
#ifdef Q_SPY
    char const *endpoint=std::getenv("FLY_QSPY_TARGET");
    if (!QS_INIT(endpoint)) {
        std::fprintf(stderr, "QS: QSpy TCP connection failed; start qspy -t first.\n");
        std::abort();
    }
    QS_GLB_FILTER(QP::QS_ALL_RECORDS);
    QS_GLB_FILTER(-QP::QS_QF_TICK);
    QS_OBJ_DICTIONARY(GAME::AO_Tunnel);
    QS_OBJ_DICTIONARY(GAME::AO_Ship);
    QS_OBJ_DICTIONARY(GAME::AO_Missile);
    QS_SIG_DICTIONARY(GAME::TIME_TICK_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::PLAYER_TRIGGER_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::PLAYER_QUIT_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::GAME_OVER_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::PLAYER_SHIP_MOVE_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::TAKE_OFF_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::HIT_WALL_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::HIT_MINE_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::MISSILE_FIRE_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::DESTROYED_MINE_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::SCORE_SIG, nullptr);
#endif
    // Generous queues and pool for a non-real-time desktop environment.
    static QF_MPOOL_EL(GAME::ObjectImageEvt) pool[(2*GAME_MINES_MAX+10)*10];
    QP::QF::poolInit(pool,sizeof(pool),sizeof(pool[0]));
    static QP::QSubscrList subscr[GAME::MAX_PUB_SIG];
    QP::QActive::psInit(subscr,Q_DIM(subscr));
    static QP::QEvt const *tunnelQueue[(GAME_MINES_MAX+5)*10];
    static QP::QEvt const *shipQueue[3*10];
    static QP::QEvt const *missileQueue[2*10];
    GAME::AO_Tunnel->start(2U,tunnelQueue,Q_DIM(tunnelQueue),nullptr,0U);
    GAME::AO_Ship->start(3U,shipQueue,Q_DIM(shipQueue),nullptr,0U);
    GAME::AO_Missile->start(4U,missileQueue,Q_DIM(missileQueue),nullptr,0U);
    qpReady.store(true,std::memory_order_release);
    QP::QF::run();
}
}

namespace BSP {
void setThrottle(bool value);
}

namespace QP {
namespace QF {
void onStartup() { setTickRate(BSP::TICKS_PER_SEC,0); }
void onCleanup() {}
void onClockTick() {
    QTimeEvt::TICK_X(0U,nullptr);
    QF::PUBLISH(&tickEvt,nullptr);
#ifdef Q_SPY
    QS_RX_INPUT();
    QS_OUTPUT();
#endif
}
}
}

int main() {
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR,"0");
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::fprintf(stderr,"SDL_Init: %s\n",SDL_GetError()); return 1;
    }
    SDL_Window *window=SDL_CreateWindow("Fly 'n' Shoot - QP/C++ / SDL3",
        FrameBridge::WIDTH*SCALE,FrameBridge::HEIGHT*SCALE,0);
    SDL_Renderer *renderer=window?SDL_CreateRenderer(window,nullptr):nullptr;
    SDL_Texture *texture=renderer?SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,
        FrameBridge::WIDTH,FrameBridge::HEIGHT):nullptr;
    if (!texture) {
        std::fprintf(stderr,"SDL setup: %s\n",SDL_GetError());
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit(); return 1;
    }
    SDL_SetTextureScaleMode(texture,SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(renderer,FrameBridge::WIDTH,
        FrameBridge::HEIGHT,SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::thread worker(qpThread);
    std::array<std::uint32_t,FrameBridge::WIDTH*FrameBridge::HEIGHT> pixels{};
    bool active=true;
    while (active) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type==SDL_EVENT_QUIT ||
                (e.type==SDL_EVENT_KEY_DOWN && e.key.key==SDLK_ESCAPE)) {
                active=false;
            }
            if (!qpReady.load(std::memory_order_acquire)) continue;
            if (e.type==SDL_EVENT_KEY_DOWN && !e.key.repeat) {
                if (e.key.key==SDLK_SPACE) publish(&triggerEvt);
                if (e.key.key==SDLK_UP) BSP::setThrottle(true);
            }
            if (e.type==SDL_EVENT_KEY_UP && e.key.key==SDLK_UP)
                BSP::setThrottle(false);
        }
        auto fb=FrameBridge::snapshot();
        for (unsigned y=0;y<FrameBridge::HEIGHT;++y)
            for (unsigned x=0;x<FrameBridge::WIDTH;++x)
                pixels[y*FrameBridge::WIDTH+x]=
                    (fb[y*4U+(x>>5U)] & (1U<<(x&31U)))
                    ?0xFF101820U:0xFFA0A0A0U;
        SDL_UpdateTexture(texture,nullptr,pixels.data(),
                          FrameBridge::WIDTH*sizeof(std::uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer,texture,nullptr,nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    if (qpReady.load(std::memory_order_acquire)) {
        publish(&quitEvt);
        QP::QF::stop();
    }
    worker.join();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

#ifdef Q_SPY
namespace QP {
void QS::onCommand(std::uint8_t cmdId, std::uint32_t p1,
                   std::uint32_t p2, std::uint32_t p3) {
    (void)cmdId; (void)p1; (void)p2; (void)p3;
}
}
#endif
