#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../utils/sort.hpp"
#include "../ray_ubo.hpp"
#include "transform.hpp"

#include <vector>
#include <memory>
#include <algorithm>
#include <stack>

namespace nugiEngine {
  const glm::vec3 eps(0.0001f);
  const int splitNumber = 11;

  // Axis-aligned bounding box.
  struct Aabb {
    glm::vec3 min = glm::vec3{FLT_MAX};
    glm::vec3 max = glm::vec3{FLT_MIN};

    int longestAxis();
    int randomAxis();
  };

  // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
  struct BoundBox {
    int index;

    BoundBox(int i) : index{i} {}

    virtual Aabb boundingBox() = 0;

    virtual glm::vec3 getOriginalMin() { return glm::vec3(0.0f); }
    virtual glm::vec3 getOriginalMax() { return glm::vec3(0.0f); }
  };

  struct TriangleBoundBox : BoundBox {
    Triangle &triangle;

    TriangleBoundBox(int i, Triangle &t) : BoundBox(i), triangle{t} {}

    Aabb boundingBox();
  };

  struct SphereBoundBox : BoundBox {
    Sphere &sphere;

    SphereBoundBox(int i, Sphere &s) : BoundBox(i), sphere{s} {}

    Aabb boundingBox();
  };

  struct PrimitiveBoundBox : BoundBox {
    Primitive &primitive;

    PrimitiveBoundBox(int i, Primitive &p) : BoundBox(i), primitive{p} {}

    Aabb boundingBox();
  };

  struct ObjectBoundBox : BoundBox {
    Object &object;
    std::shared_ptr<TransformComponent> transformation;
    std::shared_ptr<std::vector<Primitive>> primitives;

    glm::vec3 originalMin{};
    glm::vec3 originalMax{};

    ObjectBoundBox(int i, Object &o, std::shared_ptr<std::vector<Primitive>> p, std::shared_ptr<TransformComponent> t);

    glm::vec3 getOriginalMin() { return this->originalMin; }
    glm::vec3 getOriginalMax() { return this->originalMax; }
    
    Aabb boundingBox();

    private:
      float findMax(uint32_t index);
      float findMin(uint32_t index);
  };

  struct LightBoundBox : BoundBox {
    Light &light;

    LightBoundBox(int i, Light &l) : BoundBox(i), light{l} {}

    Aabb boundingBox();
  };

  // Intermediate BvhNode structure needed for constructing Bvh.
  struct BvhItemBuild {
    Aabb box;
    int index = -1; // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
    int leftNodeIndex = -1;
    int rightNodeIndex = -1;
    std::vector<std::shared_ptr<BoundBox>> objects;

    BvhNode getGpuModel();
  };

  bool nodeCompare(BvhItemBuild &a, BvhItemBuild &b);
  Aabb surroundingBox(Aabb box0, Aabb box1);
  Aabb objectListBoundingBox(std::vector<std::shared_ptr<BoundBox>> &objects);
  bool boxCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b, int axis);
  bool boxXCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  bool boxYCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  bool boxZCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b);
  int findPrimitiveSplitIndex(BvhItemBuild node, int axis, float length);

  // Since GPU can't deal with tree structures we need to create a flattened BVH.
  // Stack is used instead of a tree.
  std::shared_ptr<std::vector<BvhNode>> createBvh(const std::vector<std::shared_ptr<BoundBox>> boundedBoxes);

}// namespace nugiEngine 
