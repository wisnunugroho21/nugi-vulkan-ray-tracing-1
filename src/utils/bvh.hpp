#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

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
    glm::vec3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
    glm::vec3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    int longestAxis();
    int randomAxis();
  };

  // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
  struct BoundBox {
    int index;

    BoundBox(int i) : index{i} {}

    virtual Aabb boundingBox() = 0;
  };

  struct TriangleBoundBox : BoundBox {
    std::shared_ptr<Triangle> triangles;

    TriangleBoundBox(int i, std::shared_ptr<Triangle> t) : BoundBox(i), triangles{t} {}

    Aabb boundingBox();
  };

  struct SphereBoundBox : BoundBox {
    std::shared_ptr<Sphere> spheres;

    SphereBoundBox(int i, std::shared_ptr<Sphere> s) : BoundBox(i), spheres{s} {}

    Aabb boundingBox();
  };

  struct PrimitiveBoundBox : BoundBox {
    std::shared_ptr<Primitive> primitives;

    PrimitiveBoundBox(int i, std::shared_ptr<Primitive> p) : BoundBox(i), primitives{p} {}

    Aabb boundingBox();
  };

  struct ObjectBoundBox : BoundBox {
    std::shared_ptr<Object> objects;
    std::shared_ptr<TransformComponent> transformation;
    std::vector<std::shared_ptr<Primitive>> primitives{};

    ObjectBoundBox(int i, std::shared_ptr<Object> o, std::vector<std::shared_ptr<Primitive>> p, std::shared_ptr<TransformComponent> t) : BoundBox(i), objects{o}, primitives{p}, transformation{t} {}

    Aabb boundingBox();

    private:
      float findMax(uint32_t index);
      float findMin(uint32_t index);
  };

  struct LightBoundBox : BoundBox {
    std::shared_ptr<Light> lights;

    LightBoundBox(int i, std::shared_ptr<Light> l) : BoundBox(i), lights{l} {}

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
  std::vector<std::shared_ptr<BvhNode>> createBvh(const std::vector<std::shared_ptr<BoundBox>> boundedBoxes);

}// namespace nugiEngine 
