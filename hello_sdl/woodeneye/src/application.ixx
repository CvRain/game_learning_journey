module;

#include <string_view>
#include <SDL3/SDL.h>
#include <format>
#include <cmath>
#include <algorithm>
#include <array>
#include <numbers>
#include <random>
#include <span>
#include <ranges>

export module woodeneye.application;

import woodeneye.types;

export class Application {
public:
    explicit Application(std::string_view title, int width, int height);

    ~Application();

    auto handle_event(SDL_Event *event) -> SDL_AppResult;

    auto handle_iteration() -> SDL_AppResult;

private:
    const std::string_view window_title{"WoodenEye SDL3 Application"};
    const int window_width{800};
    const int window_height{600};

    SDL_Window *window{nullptr};
    SDL_Renderer *renderer{nullptr};

    int player_count{0};
    std::array<Player, MAX_PLAYER_COUNT> players{};
    std::array<std::array<float, 6>, MAP_BOX_EDGES_LEN> edges{};

    std::mt19937 rng{std::random_device{}()};

    Uint64 accu{0};
    Uint64 last{0};
    Uint64 past{0};
    char debug_string[32]{};

    void initPlayers();

    void initEdges();

    void shoot(int shooter);

    void update(Uint64 dt_ns);

    void draw(SDL_Renderer *renderer);

    static void drawCircle(SDL_Renderer *renderer, float r, float x, float y);

    static void drawClippedSegment(SDL_Renderer *renderer, float ax, float ay, float az, float bx, float by, float bz,
                                   float x, float y, float z, float w);

    [[nodiscard]] auto active_players() -> std::span<Player> {
        return {players.data(), static_cast<size_t>(player_count)};
    }

    [[nodiscard]] auto active_players() const -> std::span<const Player> {
        return {players.data(), static_cast<size_t>(player_count)};
    }

    [[nodiscard]] auto whoseMouse(SDL_MouseID mouse_id) const -> int;

    [[nodiscard]] auto whoseKeyboard(SDL_KeyboardID keyboard_id) const -> int;
};

Application::Application(std::string_view title, int width, int height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = std::format("SDL_Init Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }

    if (not SDL_CreateWindowAndRenderer(title.data(), width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        const auto result = std::format("SDL_CreateWindowAndRenderer Error: {}", SDL_GetError());
        throw std::runtime_error(result);
    }
    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetAppMetadata("com.claude-rainer.wooden-eye", "1.0.0", "WoodenEye SDL3 Application");

    for (const auto &[key, value]: extend_metadata) {
        SDL_SetAppMetadataProperty(key.data(), value.data());
    }

    player_count = 1;
    initPlayers();
    initEdges();

    SDL_SetRenderVSync(renderer, 0);
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);
}

Application::~Application() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

auto Application::handle_event(SDL_Event *event) -> SDL_AppResult {
    int i;
    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_MOUSE_REMOVED:
            for (i = 0; i < player_count; i++) {
                if (players[i].mouse == event->mdevice.which) {
                    players[i].mouse = 0;
                }
            }
            break;
        case SDL_EVENT_KEYBOARD_REMOVED:
            for (i = 0; i < player_count; i++) {
                if (players[i].keyboard == event->kdevice.which) {
                    players[i].keyboard = 0;
                }
            }
            break;
        case SDL_EVENT_MOUSE_MOTION: {
            SDL_MouseID id = event->motion.which;
            int index = whoseMouse(id);
            if (index >= 0) {
                players[index].yaw -= ((int)event->motion.xrel) * 0x00080000;
                players[index].pitch = std::max(-0x40000000,
                                                std::min(0x40000000,
                                                         players[index].pitch - ((int)event->motion.yrel) *
                                                         0x00080000));
            }
            else if (id) {
                for (i = 0; i < MAX_PLAYER_COUNT; i++) {
                    if (players[i].mouse == 0) {
                        players[i].mouse = id;
                        player_count = std::max(player_count, i + 1);
                        break;
                    }
                }
            }
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            SDL_MouseID id = event->button.which;
            int index = whoseMouse(id);
            if (index >= 0) {
                shoot(index);
            }
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            SDL_Keycode sym = event->key.key;
            SDL_KeyboardID id = event->key.which;
            int index = whoseKeyboard(id);
            if (index >= 0) {
                if (sym == SDLK_W) players[index].wasd |= 1;
                if (sym == SDLK_A) players[index].wasd |= 2;
                if (sym == SDLK_S) players[index].wasd |= 4;
                if (sym == SDLK_D) players[index].wasd |= 8;
                if (sym == SDLK_SPACE) players[index].wasd |= 16;
            }
            else if (id) {
                for (i = 0; i < MAX_PLAYER_COUNT; i++) {
                    if (players[i].keyboard == 0) {
                        players[i].keyboard = id;
                        player_count = std::max(player_count, i + 1);
                        break;
                    }
                }
            }
            break;
        }
        case SDL_EVENT_KEY_UP: {
            SDL_Keycode sym = event->key.key;
            SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_ESCAPE) return SDL_APP_SUCCESS;
            int index = whoseKeyboard(id);
            if (index >= 0) {
                if (sym == SDLK_W) players[index].wasd &= 30;
                if (sym == SDLK_A) players[index].wasd &= 29;
                if (sym == SDLK_S) players[index].wasd &= 27;
                if (sym == SDLK_D) players[index].wasd &= 23;
                if (sym == SDLK_SPACE) players[index].wasd &= 15;
            }
            break;
        }
        default:
            break;
    }
    return SDL_APP_CONTINUE;
}

auto Application::handle_iteration() -> SDL_AppResult {
    const Uint64 now = SDL_GetTicksNS();
    const Uint64 dt_ns = now - past;
    update(dt_ns);
    draw(renderer);
    if (now - last > 999999999) {
        last = now;
        auto result = std::format_to_n(debug_string, static_cast<std::ptrdiff_t>(std::size(debug_string) - 1), "{} fps", accu);
        *result.out = '\0';
        accu = 0;
    }
    past = now;
    accu += 1;
    Uint64 elapsed = SDL_GetTicksNS() - now;
    if (elapsed < 999999) {
        SDL_DelayNS(999999 - elapsed);
    }
    return SDL_APP_CONTINUE;
}

void Application::initPlayers() {
    for (int i = 0; i < MAX_PLAYER_COUNT; i++) {
        players[i].pos[0] = 8.0 * (i & 1 ? -1.0 : 1.0);
        players[i].pos[1] = 0;
        players[i].pos[2] = 8.0 * (i & 1 ? -1.0 : 1.0) * (i & 2 ? -1.0 : 1.0);
        players[i].vel[0] = 0;
        players[i].vel[1] = 0;
        players[i].vel[2] = 0;
        players[i].yaw = 0x20000000 + (i & 1 ? 0x80000000 : 0) + (i & 2 ? 0x40000000 : 0);
        players[i].pitch = -0x08000000;
        players[i].radius = 0.5f;
        players[i].height = 1.5f;
        players[i].wasd = 0;
        players[i].mouse = 0;
        players[i].keyboard = 0;
        players[i].color[0] = (1 << (i / 2)) & 2 ? 0 : 0xff;
        players[i].color[1] = (1 << (i / 2)) & 1 ? 0 : 0xff;
        players[i].color[2] = (1 << (i / 2)) & 4 ? 0 : 0xff;
        players[i].color[0] = (i & 1) ? players[i].color[0] : ~players[i].color[0];
        players[i].color[1] = (i & 1) ? players[i].color[1] : ~players[i].color[1];
        players[i].color[2] = (i & 1) ? players[i].color[2] : ~players[i].color[2];
    }
}

void Application::initEdges() {
    int i, j;
    constexpr auto r = static_cast<float>(MAP_BOX_SCALE);
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 3; j++) {
            const int map[24] = {
                0, 1, 1, 3, 3, 2, 2, 0,
                7, 6, 6, 4, 4, 5, 5, 7,
                6, 2, 3, 7, 0, 4, 5, 1
            };
            edges[i][j + 0] = (map[i * 2 + 0] & (1 << j) ? r : -r);
            edges[i][j + 3] = (map[i * 2 + 1] & (1 << j) ? r : -r);
        }
    }
    for (i = 0; i < MAP_BOX_SCALE; i++) {
        const auto d = static_cast<float>(i * 2);
        for (j = 0; j < 2; j++) {
            edges[i + 12][3 * j + 0] = j ? r : -r;
            edges[i + 12][3 * j + 1] = -r;
            edges[i + 12][3 * j + 2] = d - r;
            edges[i + 12 + MAP_BOX_SCALE][3 * j + 0] = d - r;
            edges[i + 12 + MAP_BOX_SCALE][3 * j + 1] = -r;
            edges[i + 12 + MAP_BOX_SCALE][3 * j + 2] = j ? r : -r;
        }
    }
}

void Application::shoot(int shooter) {
    int i, j;
    double x0 = players[shooter].pos[0];
    double y0 = players[shooter].pos[1];
    double z0 = players[shooter].pos[2];
    double bin_rad = std::numbers::pi / 2147483648.0;
    double yaw_rad = bin_rad * players[shooter].yaw;
    double pitch_rad = bin_rad * players[shooter].pitch;
    double cos_yaw = std::cos(yaw_rad);
    double sin_yaw = std::sin(yaw_rad);
    double cos_pitch = std::cos(pitch_rad);
    double sin_pitch = std::sin(pitch_rad);
    double vx = -sin_yaw * cos_pitch;
    double vy = sin_pitch;
    double vz = -cos_yaw * cos_pitch;
    for (i = 0; i < player_count; i++) {
        if (i == shooter) continue;
        Player *target = &(players[i]);
        int hit = 0;
        for (j = 0; j < 2; j++) {
            double r = target->radius;
            double h = target->height;
            double dx = target->pos[0] - x0;
            double dy = target->pos[1] - y0 + (j == 0 ? 0 : r - h);
            double dz = target->pos[2] - z0;
            double vd = vx * dx + vy * dy + vz * dz;
            double dd = dx * dx + dy * dy + dz * dz;
            double vv = vx * vx + vy * vy + vz * vz;
            double rr = r * r;
            if (vd < 0) continue;
            if (vd * vd >= vv * (dd - rr)) hit += 1;
        }
        if (hit) {
            std::uniform_int_distribution<int> dist(-128, 127);
            target->pos[0] = static_cast<double>(MAP_BOX_SCALE * dist(rng)) / 256.0;
            target->pos[1] = static_cast<double>(MAP_BOX_SCALE * dist(rng)) / 256.0;
            target->pos[2] = static_cast<double>(MAP_BOX_SCALE * dist(rng)) / 256.0;
        }
    }
}

void Application::update(Uint64 dt_ns) {
    for (auto& player : active_players()) {
        double rate = 6.0;
        double time = static_cast<double>(dt_ns) * 1e-9;
        double drag = std::exp(-time * rate);
        double diff = 1.0 - drag;
        double mult = 60.0;
        double grav = 25.0;
        auto yaw = static_cast<double>(player.yaw);
        double rad = yaw * std::numbers::pi / 2147483648.0;
        double cos = std::cos(rad);
        double sin = std::sin(rad);
        unsigned char wasd = player.wasd;
        double dirX = (wasd & 8 ? 1.0 : 0.0) - (wasd & 2 ? 1.0 : 0.0);
        double dirZ = (wasd & 4 ? 1.0 : 0.0) - (wasd & 1 ? 1.0 : 0.0);
        double norm = dirX * dirX + dirZ * dirZ;
        double accX = mult * (norm == 0 ? 0 : (cos * dirX + sin * dirZ) / std::sqrt(norm));
        double accZ = mult * (norm == 0 ? 0 : (-sin * dirX + cos * dirZ) / std::sqrt(norm));
        double velX = player.vel[0];
        double velY = player.vel[1];
        double velZ = player.vel[2];
        player.vel[0] -= velX * diff;
        player.vel[1] -= grav * time;
        player.vel[2] -= velZ * diff;
        player.vel[0] += diff * accX / rate;
        player.vel[2] += diff * accZ / rate;
        player.pos[0] += (time - diff / rate) * accX / rate + diff * velX / rate;
        player.pos[1] += -0.5 * grav * time * time + velY * time;
        player.pos[2] += (time - diff / rate) * accZ / rate + diff * velZ / rate;
        auto scale = static_cast<double>(MAP_BOX_SCALE);
        double bound = scale - player.radius;
        double posX = std::max(std::min(bound, player.pos[0]), -bound);
        double posY = std::max(std::min(bound, player.pos[1]), player.height - scale);
        double posZ = std::max(std::min(bound, player.pos[2]), -bound);
        if (player.pos[0] != posX) player.vel[0] = 0;
        if (player.pos[1] != posY) player.vel[1] = (wasd & 16) ? 8.4375 : 0;
        if (player.pos[2] != posZ) player.vel[2] = 0;
        player.pos[0] = posX;
        player.pos[1] = posY;
        player.pos[2] = posZ;
    }
}

void Application::draw(SDL_Renderer *renderer) {
    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    if (player_count > 0) {
        auto wf = static_cast<float>(w);
        auto hf = static_cast<float>(h);
        int part_hor = player_count > 2 ? 2 : 1;
        int part_ver = player_count > 1 ? 2 : 1;
        float size_hor = wf / static_cast<float>(part_hor);
        float size_ver = hf / static_cast<float>(part_ver);
        for (int i = 0; i < player_count; i++) {
            const Player *player = &players[i];
            auto mod_x = static_cast<float>(i % part_hor);
            auto mod_y = static_cast<float>(i / part_hor);
            float hor_origin = (mod_x + 0.5f) * size_hor;
            float ver_origin = (mod_y + 0.5f) * size_ver;
            float cam_origin = 0.5f * std::hypot(size_hor, size_ver);
            float hor_offset = mod_x * size_hor;
            float ver_offset = mod_y * size_ver;
            SDL_Rect rect;
            rect.x = static_cast<int>(hor_offset);
            rect.y = static_cast<int>(ver_offset);
            rect.w = static_cast<int>(size_hor);
            rect.h = static_cast<int>(size_ver);
            SDL_SetRenderClipRect(renderer, &rect);
            double x0 = player->pos[0];
            double y0 = player->pos[1];
            double z0 = player->pos[2];
            double bin_rad = std::numbers::pi / 2147483648.0;
            double yaw_rad = bin_rad * player->yaw;
            double pitch_rad = bin_rad * player->pitch;
            double cos_yaw = std::cos(yaw_rad);
            double sin_yaw = std::sin(yaw_rad);
            double cos_pitch = std::cos(pitch_rad);
            double sin_pitch = std::sin(pitch_rad);
            double mat[9] = {
                cos_yaw, 0, -sin_yaw,
                sin_yaw * sin_pitch, cos_pitch, cos_yaw * sin_pitch,
                sin_yaw * cos_pitch, -sin_pitch, cos_yaw * cos_pitch
            };
            SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
            for (const auto &line : edges) {
                auto ax = static_cast<float>(mat[0] * (line[0] - x0) + mat[1] * (line[1] - y0) + mat[2] * (line[2] - z0));
                auto ay = static_cast<float>(mat[3] * (line[0] - x0) + mat[4] * (line[1] - y0) + mat[5] * (line[2] - z0));
                auto az = static_cast<float>(mat[6] * (line[0] - x0) + mat[7] * (line[1] - y0) + mat[8] * (line[2] - z0));
                auto bx = static_cast<float>(mat[0] * (line[3] - x0) + mat[1] * (line[4] - y0) + mat[2] * (line[5] - z0));
                auto by = static_cast<float>(mat[3] * (line[3] - x0) + mat[4] * (line[4] - y0) + mat[5] * (line[5] - z0));
                auto bz = static_cast<float>(mat[6] * (line[3] - x0) + mat[7] * (line[4] - y0) + mat[8] * (line[5] - z0));
                drawClippedSegment(renderer, ax, ay, az, bx, by, bz, hor_origin, ver_origin, cam_origin, 1.0f);
            }
            for (int j = 0; j < player_count; j++) {
                if (i == j) continue;
                const Player *target = &players[j];
                SDL_SetRenderDrawColor(renderer, target->color[0], target->color[1], target->color[2], 255);
                for (int k = 0; k < 2; k++) {
                    double rx = target->pos[0] - player->pos[0];
                    double ry = target->pos[1] - player->pos[1] + (target->radius - target->height) * static_cast<float>(k);
                    double rz = target->pos[2] - player->pos[2];
                    double dx = mat[0] * rx + mat[1] * ry + mat[2] * rz;
                    double dy = mat[3] * rx + mat[4] * ry + mat[5] * rz;
                    double dz = mat[6] * rx + mat[7] * ry + mat[8] * rz;
                    double r_eff = target->radius * cam_origin / dz;
                    if (!(dz < 0)) continue;
                    drawCircle(renderer, static_cast<float>(r_eff), static_cast<float>(hor_origin - cam_origin * dx / dz),
                               static_cast<float>(ver_origin + cam_origin * dy / dz));
                }
            }
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderLine(renderer, hor_origin, ver_origin - 10, hor_origin, ver_origin + 10);
            SDL_RenderLine(renderer, hor_origin - 10, ver_origin, hor_origin + 10, ver_origin);
        }
    }
    SDL_SetRenderClipRect(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 0, 0, debug_string);
    SDL_RenderPresent(renderer);
}

void Application::drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
    SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
    for (int i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
        float ang = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(CIRCLE_DRAW_SIDES);
        points[i].x = x + r * std::cos(ang);
        points[i].y = y + r * std::sin(ang);
    }
    SDL_RenderLines(renderer, (const SDL_FPoint *)&points, CIRCLE_DRAW_SIDES_LEN);
}

void Application::drawClippedSegment(SDL_Renderer *renderer, float ax, float ay, float az, float bx, float by, float bz,
                                     float x, float y, float z, float w) {
    if (az >= -w && bz >= -w) return;
    float dx = ax - bx;
    float dy = ay - by;
    if (az > -w) {
        float t = (-w - bz) / (az - bz);
        ax = bx + dx * t;
        ay = by + dy * t;
        az = -w;
    }
    else if (bz > -w) {
        float t = (-w - az) / (bz - az);
        bx = ax - dx * t;
        by = ay - dy * t;
        bz = -w;
    }
    ax = -z * ax / az;
    ay = -z * ay / az;
    bx = -z * bx / bz;
    by = -z * by / bz;
    SDL_RenderLine(renderer, x + ax, y - ay, x + bx, y - by);
}

auto Application::whoseMouse(const SDL_MouseID mouse_id) const -> int {
    auto it = std::ranges::find_if(active_players(), [mouse_id](const Player& p) {
        return p.mouse == mouse_id;
    });
    if (it != active_players().end()) {
        return static_cast<int>(std::distance(active_players().begin(), it));
    }
    return -1;
}

auto Application::whoseKeyboard(const SDL_KeyboardID keyboard_id) const -> int {
    auto it = std::ranges::find_if(active_players(), [keyboard_id](const Player& p) {
        return p.keyboard == keyboard_id;
    });
    if (it != active_players().end()) {
        return static_cast<int>(std::distance(active_players().begin(), it));
    }
    return -1;
}
