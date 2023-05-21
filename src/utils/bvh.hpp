#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../utils/sort.hpp"
#include "../ray_ubo.hpp"

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
    std::shared_ptr<Triangle> t;

    TriangleBoundBox(int i, std::shared_ptr<Triangle> t) : BoundBox(i), t{t} {}

    Aabb boundingBox();
  };

  struct SphereBoundBox : BoundBox {
    std::shared_ptr<Sphere> s;

    SphereBoundBox(int i, std::shared_ptr<Sphere> s) : BoundBox(i), s{s} {}

    Aabb boundingBox();
  };

  struct ObjectBoundBox : BoundBox {
    std::shared_ptr<Object> o;

    ObjectBoundBox(int i, std::shared_ptr<Object> o) : BoundBox(i), o{o} {}

    Aabb boundingBox();
  };

  struct LightBoundBox : BoundBox {
    std::shared_ptr<Light> l;

    LightBoundBox(int i, std::shared_ptr<Light> l) : BoundBox(i), l{l} {}

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
  int findObjectSplitIndex(BvhItemBuild node, int axis, float length);

  // Since GPU can't deal with tree structures we need to create a flattened BVH.
  // Stack is used instead of a tree.
  std::vector<BvhNode> createBvh(const std::vector<std::shared_ptr<BoundBox>> srcObjects);

}// namespace nugiEngine 
