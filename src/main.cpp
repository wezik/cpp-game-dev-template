#include <flecs.h>
#include <raylib-cpp.hpp>

#include "ball.h"
#include "wire.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

JPH::BodyID CreateCircle(JoltPhysicsWorld &world, float x, float y, float radius, bool dynamic) {
    auto shape = new JPH::SphereShape(radius);

    JPH::BodyCreationSettings settings(shape,
        JPH::Vec3(x, y, 0),
        JPH::Quat::sIdentity(),
        dynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static,
        JPH::ObjectLayer(0));

    settings.mAllowedDOFs = JPH::EAllowedDOFs::Plane2D;

    auto &bi = world.get_body_interface();
    JPH::BodyID id = bi.CreateAndAddBody(settings, JPH::EActivation::Activate);
    return id;
}

int main() {
    InitWindow(800, 600, "Raylib, Jolt, and Flecs POC");
    SetTargetFPS(-1);

    auto physics = Provide::physics();
    flecs::world ecs;

    // custom pipelines
    auto FixedUpdate = ecs.entity("FixedUpdate");
    auto FrameUpdate = ecs.entity("FrameUpdate");

    auto fixed_pipeline = ecs.pipeline().with(flecs::System).with(FixedUpdate).build();

    auto frame_pipeline = ecs.pipeline().with(flecs::System).with(FrameUpdate).build();

    // disable gravity
    physics->get_system().SetGravity(JPH::Vec3(0, 0, 0));

    // sync physics to ecs
    ecs.system<DynamicCircleBody, Transform>()
        .kind(FixedUpdate)
        .each([&](DynamicCircleBody &pb, Transform &tr) {
            auto &bodyLock = physics->get_system().GetBodyLockInterfaceNoLock();
            JPH::BodyLockRead lock(bodyLock, pb.physics_id);

            if (lock.Succeeded()) {
                const JPH::Body &body = lock.GetBody();
                auto transform = body.GetWorldTransform();
                tr.translation.x = transform.GetTranslation().GetX();
                tr.translation.y = transform.GetTranslation().GetY();
            } else {
            }
        });

    // listen for added bodies
    ecs.observer<DynamicCircleBody, Transform>()
        .event(flecs::OnSet)
        .each([&](DynamicCircleBody &body, Transform &t) {
            auto x = t.translation.x;
            auto y = t.translation.y;
            body.physics_id = CreateCircle(*physics, x, y, body.radius, true);
        });

    // spawn random balls heading toward center
    auto &bi = physics->get_body_interface();
    int ball_count = GetRandomValue(20, 30);
    float center_x = 400.0f;
    float center_y = 300.0f;

    for (int i = 0; i < ball_count; i++) {
        float radius = GetRandomValue(8, 16);
        float angle = GetRandomValue(0, 360) * DEG2RAD;
        float dist = GetRandomValue(250, 350);
        float x = center_x + cosf(angle) * dist;
        float y = center_y + sinf(angle) * dist;

        float dx = center_x - x;
        float dy = center_y - y;
        float len = sqrtf(dx * dx + dy * dy);
        float speed = GetRandomValue(150, 300);
        float vx = (dx / len) * speed;
        float vy = (dy / len) * speed;

        raylib::Color color = {(unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            (unsigned char)GetRandomValue(50, 255),
            255};

        auto ball = ecs.entity()
                        .set<Transform>({{x, y, 0}, {}, {1, 1, 1}})
                        .set<DynamicCircleBody>({{}, radius})
                        .set<DrawableCircle>({radius, color});

        bi.SetLinearVelocity(ball.get<DynamicCircleBody>().physics_id, JPH::Vec3(vx, vy, 0));
    }

    const float FIXED_DT = 1.0f / 60.0f;
    const int MAX_STEPS = 5;
    float accumulator = 0.0f;

    // game loop
    while (!WindowShouldClose()) {
        float frame_time = GetFrameTime();

        if (raylib::Keyboard::IsKeyPressed(KEY_SPACE)) {
            float radius = 32.0f;
            auto drawable_ball = DrawableCircle{radius, raylib::RED};
            auto mouse_pos = GetMousePosition();
            auto translation = Vector2{mouse_pos.x, mouse_pos.y};
            auto physical_ball = DynamicCircleBody({}, radius);
            auto transform = Transform{Vector3(translation.x, translation.y, 0.0f),
                Quaternion(),
                Vector3(1.0f, 1.0f, 1.0f)};

            ecs.entity()
                .set<Transform>(transform)
                .set<DynamicCircleBody>(physical_ball)
                .set<DrawableCircle>(drawable_ball);
        }
        accumulator += frame_time;

        // update the rest (events, reactions, etc.)
        ecs.progress(frame_time);

        // fixed update
        int steps = 0;
        while (accumulator >= FIXED_DT && steps < MAX_STEPS) {
            physics->update(FIXED_DT);
            ecs.run_pipeline(fixed_pipeline, FIXED_DT);
            accumulator -= FIXED_DT;
            steps++;
        }

        // frame update
        ecs.run_pipeline(frame_pipeline, frame_time);

        // rendering
        BeginDrawing();
        ClearBackground(raylib::RAYWHITE);

        ecs.each<DrawableCircle>([](flecs::entity e, DrawableCircle &dc) {
            auto t = e.get<Transform>();
            DrawCircleV(Vector2(t.translation.x, t.translation.y), dc.radius, dc.color);
        });

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
