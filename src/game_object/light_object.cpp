#include "light_object.hpp"

namespace nugiEngine {
  EngineLightObject EngineLightObject::createLightObject() {
    static id_t currentId = 0;
    return EngineLightObject{currentId++};
  }

  std::shared_ptr<EngineLightObject> EngineLightObject::createSharedLightObject() {
    static id_t currentId = 0;
    return std::make_shared<EngineLightObject>(currentId++);
  }
} // namespace nugiEngine
