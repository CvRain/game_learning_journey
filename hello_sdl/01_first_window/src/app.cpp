// SDL3 Callback 模式入口点
// 这种设计允许 SDL3 根据平台需求控制主循环
// 对 iOS/Android/Emscripten 等平台有更好的支持

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <memory>
#include "application.hpp"

// 初始化回调：应用启动时调用一次
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    // 使用 unique_ptr 创建 Application，确保异常安全
    auto app = std::make_unique<Application>();
    if (!app->init()) {
        std::cerr << "Application init failed!" << std::endl;
        // unique_ptr 自动释放，无需手动 delete
        return SDL_APP_FAILURE;
    }
    // 使用 release() 转移所有权给 SDL callback 系统
    // 裸指针存储在 void* 中，在 SDL_AppQuit 时重新包装回 unique_ptr
    *appstate = app.release();
    return SDL_APP_CONTINUE; /* 继续运行程序 */
}

// 事件回调：每个事件触发时调用
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    auto *app = static_cast<Application *>(appstate);

    // SDL3: SDL_EVENT_QUIT 替代 SDL_QUIT
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS; // 成功退出
    }

    app->handleEvents(event);
    return SDL_APP_CONTINUE;
}

// 迭代回调：每帧调用（替代传统的 while 循环）
SDL_AppResult SDL_AppIterate(void *appstate) {
    auto *app = static_cast<Application *>(appstate);
    return app->update();
}

// 退出回调：应用结束时清理资源
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    // 将 void* 转换回 Application* 并包装为 unique_ptr
    // unique_ptr 析构时会自动调用 delete，确保资源正确释放
    std::unique_ptr<Application> app(static_cast<Application *>(appstate));
    // app 离开作用域时自动释放，无需显式 delete
}
