module;
#include <SDL3/SDL.h>
#include <format>
#include <string_view>

export module Lines.Application;

export class Application {
public:
    explicit Application(const std::string_view &title, int width, int height);
    ~Application();
    auto update() -> SDL_AppResult;
    auto handle_event(const SDL_Event *event) -> void;


private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    const std::string_view window_title;
    const int window_width;
    const int window_height;
    std::array<SDL_FPoint, 100> points{};
};

Application::Application(const std::string_view &title, const int width, const int height) :
    window_title(title), window_width(width), window_height(height) {
    if (not SDL_Init(SDL_INIT_VIDEO)) {
        const auto result = "SDL initialization failed: " + std::string(SDL_GetError());
        throw std::runtime_error(result);
    }
    if (not SDL_CreateWindowAndRenderer(window_title.data(), window_width, window_height, SDL_WINDOW_RESIZABLE, &window,
                                        &renderer)) {
        const auto result = "SDL CreateWindowAndRenderer failed: " + std::string(SDL_GetError());
        SDL_Quit();
        throw std::runtime_error(result);
    }
    SDL_SetRenderLogicalPresentation(renderer, window_width, window_height, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    for (auto &[x, y]: points) {
        x = (SDL_randf() * window_width);
        y = (SDL_randf() * window_height);
    }
}

Application::~Application() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

auto Application::update() -> SDL_AppResult {
    // a basic window with background color which like night
    SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
    SDL_RenderClear(renderer);

    const auto ticks = SDL_GetTicks();
    const auto current_time = static_cast<double>(ticks) / 1000.0f;

    // Draw twinkling stars in background
    for (size_t i = 0; i < points.size(); ++i) {
        const auto point_offset = static_cast<double>(i) / points.size();
        const auto time_offset = current_time * 0.5;

        const auto red = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D)));
        const auto green = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D * 2.0)));
        const auto blue = static_cast<Uint8>(255 * (0.7 + 0.3 * SDL_sin(time_offset + point_offset * SDL_PI_D * 3.0)));
        const auto alpha = static_cast<Uint8>(128 + 127 * SDL_sin(time_offset * 2.0 + point_offset * SDL_PI_D * 2.0));

        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);

        // Draw larger stars using small cross pattern
        const float x = points[i].x;
        const float y = points[i].y;
        const float size = 2.0f;

        // Draw center point
        SDL_RenderPoint(renderer, x, y);
        // Draw cross arms
        SDL_RenderLine(renderer, x - size, y, x + size, y);
        SDL_RenderLine(renderer, x, y - size, x, y + size);
        // Add diagonal lines for sparkle effect
        SDL_RenderLine(renderer, x - size * 0.7f, y - size * 0.7f, x + size * 0.7f, y + size * 0.7f);
        SDL_RenderLine(renderer, x - size * 0.7f, y + size * 0.7f, x + size * 0.7f, y - size * 0.7f);
    }

    // Draw Christmas Tree
    const float centerX = window_width / 2.0f;
    const float baseY = window_height - 100.0f;

    // Draw tree trunk (brown)
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    for (int i = 0; i < 30; i++) {
        SDL_RenderLine(renderer, centerX - 15, baseY - i, centerX + 15, baseY - i);
    }

    // Draw three layers of tree foliage (green triangles)
    SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);

    // Bottom layer - widest at bottom, narrower at top
    float layerBottom = baseY - 30;
    float layerHeight = 80;
    float maxWidth = 120;
    for (float y = layerBottom - layerHeight; y < layerBottom; y += 1.0f) {
        float progress = (layerBottom - y) / layerHeight; // 1.0 at top, 0.0 at bottom
        float width = maxWidth * (1.0f - progress); // wider at bottom
        SDL_RenderLine(renderer, centerX - width, y, centerX + width, y);
    }

    // Middle layer
    layerBottom = baseY - 90; // start above bottom layer
    layerHeight = 70;
    maxWidth = 100;
    for (float y = layerBottom - layerHeight; y < layerBottom; y += 1.0f) {
        float progress = (layerBottom - y) / layerHeight;
        float width = maxWidth * (1.0f - progress);
        SDL_RenderLine(renderer, centerX - width, y, centerX + width, y);
    }

    // Top layer
    layerBottom = baseY - 140; // start above middle layer
    layerHeight = 60;
    maxWidth = 80;
    for (float y = layerBottom - layerHeight; y < layerBottom; y += 1.0f) {
        float progress = (layerBottom - y) / layerHeight;
        float width = maxWidth * (1.0f - progress);
        SDL_RenderLine(renderer, centerX - width, y, centerX + width, y);
    }

    // Draw star on top (golden yellow, with animation)
    const float starPulse = 0.5f + 0.5f * SDL_sin(current_time * 3.0);
    const Uint8 starBrightness = static_cast<Uint8>(200 + 55 * starPulse);
    SDL_SetRenderDrawColor(renderer, starBrightness, starBrightness, 50, 255);

    float starTop = baseY - 210;
    float starSize = 20;
    // Draw a 5-pointed star
    SDL_RenderLine(renderer, centerX, starTop, centerX - starSize * 0.3f, starTop + starSize * 0.8f);
    SDL_RenderLine(renderer, centerX - starSize * 0.3f, starTop + starSize * 0.8f, centerX - starSize,
                   starTop + starSize * 0.3f);
    SDL_RenderLine(renderer, centerX - starSize, starTop + starSize * 0.3f, centerX - starSize * 0.5f,
                   starTop + starSize * 1.2f);
    SDL_RenderLine(renderer, centerX - starSize * 0.5f, starTop + starSize * 1.2f, centerX, starTop + starSize);
    SDL_RenderLine(renderer, centerX, starTop + starSize, centerX + starSize * 0.5f, starTop + starSize * 1.2f);
    SDL_RenderLine(renderer, centerX + starSize * 0.5f, starTop + starSize * 1.2f, centerX + starSize,
                   starTop + starSize * 0.3f);
    SDL_RenderLine(renderer, centerX + starSize, starTop + starSize * 0.3f, centerX + starSize * 0.3f,
                   starTop + starSize * 0.8f);
    SDL_RenderLine(renderer, centerX + starSize * 0.3f, starTop + starSize * 0.8f, centerX, starTop);

    // Draw colorful twinkling light bulbs
    const int ornamentPositions[][2] = {{-70, -80}, {70, -80},   {-50, -120}, {50, -120}, {0, -150},
                                        {-80, -40}, {80, -40},   {-40, -170}, {40, -170}, {-30, -60},
                                        {30, -60},  {-60, -140}, {60, -140},  {0, -100},  {-90, -100}};

    for (size_t i = 0; i < sizeof(ornamentPositions) / sizeof(ornamentPositions[0]); ++i) {
        const auto &pos = ornamentPositions[i];
        const float ornX = centerX + pos[0];
        const float ornY = baseY + pos[1];

        // Different phase for each bulb to create asynchronous twinkling
        const float phase = i * 0.7f;
        const float twinkle = 0.3f + 0.7f * SDL_sin(current_time * 3.0 + phase);

        // Cycle through rainbow colors based on position
        const float hue = (i * 0.4f + current_time * 0.3f);
        Uint8 r, g, b;

        // Create rainbow colors
        const int colorIndex = static_cast<int>(hue * 10.0f) % 7;
        switch (colorIndex) {
            case 0: // Red
                r = static_cast<Uint8>(255 * twinkle);
                g = static_cast<Uint8>(50 * twinkle);
                b = static_cast<Uint8>(50 * twinkle);
                break;
            case 1: // Orange
                r = static_cast<Uint8>(255 * twinkle);
                g = static_cast<Uint8>(165 * twinkle);
                b = static_cast<Uint8>(50 * twinkle);
                break;
            case 2: // Yellow
                r = static_cast<Uint8>(255 * twinkle);
                g = static_cast<Uint8>(255 * twinkle);
                b = static_cast<Uint8>(50 * twinkle);
                break;
            case 3: // Green
                r = static_cast<Uint8>(50 * twinkle);
                g = static_cast<Uint8>(255 * twinkle);
                b = static_cast<Uint8>(50 * twinkle);
                break;
            case 4: // Cyan
                r = static_cast<Uint8>(50 * twinkle);
                g = static_cast<Uint8>(255 * twinkle);
                b = static_cast<Uint8>(255 * twinkle);
                break;
            case 5: // Blue
                r = static_cast<Uint8>(50 * twinkle);
                g = static_cast<Uint8>(100 * twinkle);
                b = static_cast<Uint8>(255 * twinkle);
                break;
            case 6: // Purple
                r = static_cast<Uint8>(200 * twinkle);
                g = static_cast<Uint8>(50 * twinkle);
                b = static_cast<Uint8>(255 * twinkle);
                break;
            default:
                r = g = b = static_cast<Uint8>(255 * twinkle);
        }

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        // Draw bulb shape (circle filled with lines)
        const float bulbRadius = 6.0f;
        for (float angle = 0; angle < 2 * SDL_PI_D; angle += 0.2f) {
            const float x1 = ornX + bulbRadius * SDL_cos(angle);
            const float y1 = ornY + bulbRadius * SDL_sin(angle);
            const float x2 = ornX + bulbRadius * SDL_cos(angle + 0.2f);
            const float y2 = ornY + bulbRadius * SDL_sin(angle + 0.2f);
            SDL_RenderLine(renderer, x1, y1, x2, y2);
        }

        // Add bright center highlight when bulb is bright
        if (twinkle > 0.7f) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
            for (float angle = 0; angle < 2 * SDL_PI_D; angle += 0.3f) {
                const float x1 = ornX + 2.5f * SDL_cos(angle);
                const float y1 = ornY + 2.5f * SDL_sin(angle);
                const float x2 = ornX + 2.5f * SDL_cos(angle + 0.3f);
                const float y2 = ornY + 2.5f * SDL_sin(angle + 0.3f);
                SDL_RenderLine(renderer, x1, y1, x2, y2);
            }
        }
    }

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

auto Application::handle_event(const SDL_Event *event) -> void {
    // currently no event to handle
}
