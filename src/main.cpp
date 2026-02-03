// #include <iostream>
#include <raylib-cpp.hpp>
#include <flecs.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/RegisterTypes.h>

#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <raylib.h>
#include <memory>

using namespace JPH;

struct WorldTransform {
    Vector2 pos;
};

struct PhysicsBody {
    JPH::BodyID id;
};

namespace BroadPhaseLayers {
    static constexpr JPH::BroadPhaseLayer MOVING(0);
    static constexpr uint32 NUM_LAYERS = 1;
};

class BPLayerInterface final : public JPH::BroadPhaseLayerInterface {
public:
    JPH::uint GetNumBroadPhaseLayers() const override {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer) const override {
        return BroadPhaseLayers::MOVING;
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer) const override {
        return "MOVING";
    }
#endif

};

class ObjectVsBPLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::BroadPhaseLayer) const override {
        return true;
    }
};

class ObjectvsBPLayerPairFilter final : public JPH::ObjectLayerPairFilter {
public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::ObjectLayer) const override {
        return true;
    }
};

struct PhysicsWorld {
    JPH::PhysicsSystem system;
    std::unique_ptr<JPH::TempAllocatorImpl> tempAllocator;
    std::unique_ptr<JPH::JobSystemSingleThreaded> jobSystem;

    BPLayerInterface brodPhase;
    ObjectVsBPLayerFilter filter;
    ObjectvsBPLayerPairFilter pairFilter;

    PhysicsWorld() {
        JPH::RegisterDefaultAllocator();
        JPH::Factory::sInstance = new JPH::Factory();
        JPH::RegisterTypes();

        tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(1024 * 1024);
        jobSystem = std::make_unique<JPH::JobSystemSingleThreaded>(JPH::cMaxPhysicsJobs);

        constexpr uint32_t maxBodies = 1024;
        constexpr uint32_t numBodyMutexes = 0;
        constexpr uint32_t maxBodyPairs = 1024;
        constexpr uint32_t maxContactConstraints = 1024;

        system.Init(
            maxBodies,
            numBodyMutexes,
            maxBodyPairs,
            maxContactConstraints,
            brodPhase,
            filter,
            pairFilter
        );
    }

    void Update(float dt) {
        system.Update(dt, 1, tempAllocator.get(), jobSystem.get());
    }

    ~PhysicsWorld() {
        JPH::UnregisterTypes();
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    }
};

JPH::BodyID CreateCircle(
    PhysicsWorld& world,
    float x, 
    float y,
    float radius,
    bool dynamic
) {
    auto shape = new JPH::SphereShape(radius);

    JPH::BodyCreationSettings settings(
        shape,
        JPH::Vec3(x, y, 0),
        JPH::Quat::sIdentity(),
        dynamic ? JPH::EMotionType::Dynamic : JPH::EMotionType::Static,
        JPH::ObjectLayer(0)
    );

    settings.mAllowedDOFs = JPH::EAllowedDOFs::Plane2D;

    auto& bi = world.system.GetBodyInterface();
    JPH::BodyID id = bi.CreateAndAddBody(settings, JPH::EActivation::Activate);
    return id;
}

int main() {
    InitWindow(800, 600, "Raylib, Jolt, and Flecs POC");
    SetTargetFPS(60);

    PhysicsWorld physics;
    flecs::world ecs;

    // disable gravity
    physics.system.SetGravity(JPH::Vec3(0, 0, 0));

    // create entities for ecs and physics
    auto entity1 = ecs.entity()
        .set<WorldTransform>({ {200, 300} })
        .set<PhysicsBody>({
            CreateCircle(physics, 200, 300, 30.0f, true)
        });

    auto entity2 = ecs.entity()
        .set<WorldTransform>({ {600, 300} })
        .set<PhysicsBody>({
            CreateCircle(physics, 600, 300, 30.0f, true)
        });

    ecs.entity()
        .set<WorldTransform>({ {400, 300} })
        .set<PhysicsBody>({
            CreateCircle(physics, 400, 290, 30.0f, false)
        });

    const float fixed_dt = 1.0f / 60.0f;

    // define ecs system for syncing physics to transforms
    ecs.system<PhysicsBody, WorldTransform>()
        .kind(flecs::OnUpdate)
        .each([&](PhysicsBody& pb, WorldTransform& tr) {
            auto& bodyLock = physics.system.GetBodyLockInterfaceNoLock();
            JPH::BodyLockRead lock(bodyLock, pb.id);

            if (lock.Succeeded()) {
                const JPH::Body& body = lock.GetBody();
                auto transform = body.GetWorldTransform();
                tr.pos.x = transform.GetTranslation().GetX();
                tr.pos.y = transform.GetTranslation().GetY();
            }
        });

    // set initial velocity
    auto& bi = physics.system.GetBodyInterface();

    bi.SetLinearVelocity(entity1.get<PhysicsBody>().id, JPH::Vec3(150.0f, -1.2f, 0));
    bi.SetLinearVelocity(entity2.get<PhysicsBody>().id, JPH::Vec3(-150.0f, 1.2f, 0));

    // game loop
    while (!WindowShouldClose()) {
        physics.Update(fixed_dt);
        ecs.progress();

        BeginDrawing();
        ClearBackground(raylib::RAYWHITE);

        ecs.each<WorldTransform>([](WorldTransform& tr) {
            DrawCircleV(tr.pos, 30.0f, raylib::RED);
        });
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
