module;

#include <entt/entt.hpp>

export module snake;
export struct Position {
    int x;
    int y;
};

export struct SnakeSegment {
    int age;
};

export struct Food {};

export struct Direction {
    int dx;
    int dy;
};

export struct SnakeHead {};
