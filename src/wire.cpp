#include "wire.h"

namespace Provide {
std::unique_ptr<JoltPhysicsWorld> physics() { return std::make_unique<JoltPhysicsWorld>(); }
} // namespace Provide
