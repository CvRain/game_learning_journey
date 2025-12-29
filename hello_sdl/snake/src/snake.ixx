module;

#include <entt/entt.hpp>

export module snake;

export using Position = struct {
    int x;
    int y;
};

export using SnakeSegment = struct {
    int age;
};

export using Food = struct {};

export using Direction = struct {
    int dx;
    int dy;
};

export using SnakeHead = struct {};
