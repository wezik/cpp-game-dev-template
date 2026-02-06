#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <raylib.h>

struct DynamicCircleBody {
    JPH::BodyID physics_id;
    float radius;
};

struct DrawableCircle {
    float radius;
    Color color;
};

struct Ball {
    Transform transform;
    DynamicCircleBody body;
    DrawableCircle drawable;
};
