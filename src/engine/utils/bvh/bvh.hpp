#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../sort/sort.hpp"
#include "../transform/transform.hpp"
#include "../../general_struct.hpp"

#include <vector>
#include <memory>
#include <algorithm>
#include <stack>

namespace nugiEngine {
  const glm::vec3 eps(0.1f);
  const uint32_t splitNumber = 11;

  // Axis-aligned bounding box.
  struct Aabb {
    glm::vec3 min = glm::vec3{FLT_MAX};
    glm::vec3 max = glm::vec3{FLT_MIN};

    uint32_t longestAxis();
    uint32_t randomAxis();
  };

  // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
  struct BoundBox {
    uint32_t index;

    BoundBox(uint32_t i) : index{i} {}

    virtual Aabb boundingBox() = 0;

    virtual glm::vec3 getOriginalMin() { return glm::vec3(0.0f); }
    virtual glm::vec3 getOriginalMax() { return glm::vec3(0.0f); }
  };

  struct PrimitiveBoundBox : BoundBox {
    Primitive &primitive;
    std::shared_ptr<std::vector<Vertex>> vertices;

    PrimitiveBoundBox(uint32_t i, Primitive &p, std::shared_ptr<std::vector<Vertex>> v) : BoundBox(i), primitive{p}, vertices{v} {}

    Aabb boundingBox();
  };

  struct ObjectBoundBox : BoundBox {
    Object &object;
    std::shared_ptr<TransformComponent> transformation;
    std::shared_ptr<std::vector<Primitive>> primitives;
    std::shared_ptr<std::vector<Vertex>> vertices;

    glm::vec3 originalMin{};
    glm::vec3 originalMax{};

    ObjectBoundBox(uint32_t i, Object &o, std::shared_ptr<std::vector<Primitive>> p, std::shared_ptr<TransformComponent> t, std::shared_ptr<std::vector<Vertex>> v);

    glm::vec3 getOriginalMin() { return this->originalMin; }
    glm::vec3 getOriginalMax() { return this->originalMax; }
    
    Aabb boundingBox();

    private:
      float findMax(uint32_t index);
      float findMin(uint32_t index);
  };

  struct PointLightBoundBox : BoundBox {
    PointLight &light;

    PointLightBoundBox(int i, PointLight &l) : BoundBox(i), light{l} {}

    Aabb boundingBox();
  };

  struct AreaLightBoundBox : BoundBox {
    AreaLight &light;

    AreaLightBoundBox(int i, AreaLight &l) : BoundBox(i), light{l} {}

    Aabb boundingBox();
  };

  // Intermediate BvhNode structure needed for constructing Bvh.
  struct BvhItemBuild {
    Aabb box;
    uint32_t index = 0; // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
    uint32_t leftNodeIndex = 0;
    uint32_t rightNodeIndex = 0;
    std::vector<std::shared_ptr<BoundBox>> objects;

    BvhNode getGpuModel();
  };

  bool nodeCompare(BvhItemBuild &a, BvhItemBuild &b);
  Aabb surroundingBox(Aabb box0, Aabb box1);
  Aabb objectListBoundingBox(std::vector<std::shared_ptr<BoundBox>> &objects);
  bool boxCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b, uint32_t axis);
  bool boxXCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  bool boxYCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  bool boxZCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  uint32_t findPrimitiveSplitIndex(BvhItemBuild node, uint32_t axis, float length);

  // Since GPU can't deal with tree structures we need to create a flattened BVH.
  // Stack is used instead of a tree.
  std::shared_ptr<std::vector<BvhNode>> createBvh(const std::vector<std::shared_ptr<BoundBox>> boundedBoxes);

}// namespace nugiEngine 
