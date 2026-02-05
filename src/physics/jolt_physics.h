#pragma once

// clang-format off
#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemSingleThreaded.h>
// clang-format on

#include <memory>

class JoltPhysicsWorld final {
  public:
    JoltPhysicsWorld();
    ~JoltPhysicsWorld();

    void update(float fixed_dt);

    JPH::PhysicsSystem &get_system() { return *system; }
    JPH::BodyInterface &get_body_interface() { return system->GetBodyInterface(); }

  private:
    std::unique_ptr<JPH::PhysicsSystem> system;
    std::unique_ptr<JPH::TempAllocator> temp_allocator;
    std::unique_ptr<JPH::JobSystemSingleThreaded> job_system;
};
