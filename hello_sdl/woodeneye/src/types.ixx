module;
#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>

export module woodeneye.types;

export constexpr int MAP_BOX_SCALE = 24;
export constexpr int MAP_BOX_EDGES_LEN = (12 + MAP_BOX_SCALE * 2);
export constexpr int MAX_PLAYER_COUNT = 4;
export constexpr int CIRCLE_DRAW_SIDES = 32;
export constexpr int CIRCLE_DRAW_SIDES_LEN = (CIRCLE_DRAW_SIDES + 1);

export using Player = struct {
    SDL_MouseID mouse;
    SDL_KeyboardID keyboard;
    std::array<double, 3> pos;
    std::array<double, 3> vel;
    unsigned int yaw;
    int pitch;
    float radius;
    float height;
    std::array<unsigned char, 3> color;
    unsigned char wasd;
};

export const std::unordered_map<std::string_view, std::string_view> extend_metadata{
    {SDL_PROP_APP_METADATA_URL_STRING, "https://examples.libsdl.org/SDL3/demo/02-woodeneye-008/"},
    {SDL_PROP_APP_METADATA_CREATOR_STRING, "SDL team"},
    {SDL_PROP_APP_METADATA_COPYRIGHT_STRING, "Placed in the public domain"},
    {SDL_PROP_APP_METADATA_TYPE_STRING, "game"}
};
