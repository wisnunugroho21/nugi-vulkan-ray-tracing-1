#include "game_object.hpp"

namespace nugiEngine {
  EngineGameObject EngineGameObject::createGameObject() {
    static id_t currentId = 0;
    return EngineGameObject{currentId++};
  }

  std::shared_ptr<EngineGameObject> EngineGameObject::createSharedGameObject() {
    static id_t currentId = 0;
    return std::make_shared<EngineGameObject>(currentId++);
  }

} // namespace nugiEngine
