#include "jolt_physics.h"

#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/RegisterTypes.h>
#include <memory>

namespace {

constexpr JPH::BroadPhaseLayer MOVING_LAYER(0);
constexpr uint32_t NUM_BROAD_PHASE_LAYERS = 1;

class BPLayer final : public JPH::BroadPhaseLayerInterface {
  public:
    uint GetNumBroadPhaseLayers() const override { return NUM_BROAD_PHASE_LAYERS; }

    JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer) const override {
        return MOVING_LAYER;
    }
};

class ObjectBPLayerFilter final : public JPH::ObjectVsBroadPhaseLayerFilter {
  public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::BroadPhaseLayer) const override { return true; }
};

class ObjectPairFilter final : public JPH::ObjectLayerPairFilter {
  public:
    bool ShouldCollide(JPH::ObjectLayer, JPH::ObjectLayer) const override { return true; }
};

// static instances for PhysicsSystem init
static BPLayer s_broad_phase;
static ObjectBPLayerFilter s_layer_filter;
static ObjectPairFilter s_pair_filter;

} // namespace

JoltPhysicsWorld::JoltPhysicsWorld() {
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    temp_allocator = std::make_unique<JPH::TempAllocatorImpl>(1024 * 1024);
    job_system = std::make_unique<JPH::JobSystemSingleThreaded>(JPH::cMaxPhysicsJobs);

    constexpr uint32_t max_bodies = 1024;
    constexpr uint32_t num_body_mutexes = 0;
    constexpr uint32_t max_body_pairs = 1024;
    constexpr uint32_t max_contact_constraints = 1024;

    system = std::make_unique<JPH::PhysicsSystem>();
    system->Init(max_bodies,
        num_body_mutexes,
        max_body_pairs,
        max_contact_constraints,
        s_broad_phase,
        s_layer_filter,
        s_pair_filter);
}

JoltPhysicsWorld::~JoltPhysicsWorld() {
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void JoltPhysicsWorld::update(float fixed_dt) {
    system->Update(fixed_dt, 1, temp_allocator.get(), job_system.get());
}
