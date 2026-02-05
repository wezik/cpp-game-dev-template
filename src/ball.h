#pragma once

#include <raylib-cpp.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

struct DynamicCircleBody {
    JPH::BodyID physics_id;
    float radius;
};

struct DrawableCircle {
    float radius;
    raylib::Color color;
};

struct Ball {
    Transform transform;
    DynamicCircleBody body;
    DrawableCircle drawable;
};
