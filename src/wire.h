#pragma once

#include "physics/jolt_physics_world.h"

#include <memory>

namespace Provide {
// NOTE: For now direct types are returned, it is done to favor domain exploration.
// In the future, the underlying implementations should be swappable.
std::unique_ptr<JoltPhysicsWorld> physics();
}
