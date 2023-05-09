#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../utils/sort.hpp"

#include <vector>
#include <algorithm>
#include <stack>

namespace nugiEngine {
  const glm::vec3 eps(0.0001f);
  const int splitNumber = 9;

  // Axis-aligned bounding box.
  struct Aabb {
    glm::vec3 min = {FLT_MAX, FLT_MAX, FLT_MAX};
    glm::vec3 max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

    int longestAxis() {
      float x = abs(max[0] - min[0]);
      float y = abs(max[1] - min[1]);
      float z = abs(max[2] - min[2]);

      int longest = 0;
      if (y > x && y > z) {
        longest = 1;
      }

      if (z > x && z > y) {
        longest = 2;
      }

      return longest;
    }

    int randomAxis() {
      return rand() % 3;
    }
  };

  // Utility structure to keep track of the initial triangle index in the triangles array while sorting.
  struct TriangleBoundBox {
    int index;
    Triangle t;
  };

  struct SphereBoundBox {
    int index;
    Sphere s;
  };

  struct ObjectBoundBox {
    int index;
    Object o;
  };

  // Intermediate BvhNode structure needed for constructing Bvh.
  struct BvhItemBuild {
    Aabb box;
    int index = -1; // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
    int leftNodeIndex = -1;
    int rightNodeIndex = -1;
    std::vector<ObjectBoundBox> objects;

    BvhNode getGpuModel() {
      bool leaf = leftNodeIndex == -1 && rightNodeIndex == -1;

      BvhNode node{};
      node.minimum = box.min;
      node.maximum = box.max;      

      if (leaf) {
        node.leftObjIndex = objects[0].index;

        if (objects.size() > 1) {
          node.rightObjIndex = objects[1].index;
        }
      }
      else {
        node.leftNode = leftNodeIndex;
        node.rightNode = rightNodeIndex;
      }

      return node;
    }
  };

  bool nodeCompare(BvhItemBuild &a, BvhItemBuild &b) {
    return a.index < b.index;
  }

  Aabb surroundingBox(Aabb box0, Aabb box1) {
    return Aabb{ glm::min(box0.min, box1.min), glm::max(box0.max, box1.max) };
  }

  Aabb objectBoundingBox(Object &o) {
    // Need to add eps to correctly construct an AABB for flat objects like planes.
    return Aabb{ glm::min(glm::min(o.triangle.point0, o.triangle.point1), o.triangle.point2) - eps, 
      glm::max(glm::max(o.triangle.point0, o.triangle.point1), o.triangle.point2) + eps };
    // return {t.center - t.radius - eps, t.center + t.radius + eps};
  }

  Aabb objectListBoundingBox(std::vector<ObjectBoundBox> &objects) {
    Aabb tempBox;
    Aabb outputBox;
    bool firstBox = true;

    for (auto &object : objects) {
      tempBox = objectBoundingBox(object.o);
      outputBox = firstBox ? tempBox : surroundingBox(outputBox, tempBox);
      firstBox = false;
    }

    return outputBox;
  }

  inline bool boxCompare(Object &a, Object &b, int axis) {
    Aabb boxA = objectBoundingBox(a);
    Aabb boxB = objectBoundingBox(b);

    float Apos = (boxA.max[axis] - boxA.min[axis]) / 2 + boxA.min[axis];
    float Bpos = (boxB.max[axis] - boxB.min[axis]) / 2 + boxB.min[axis];

    return Apos < Bpos;
  }

  bool boxXCompare(ObjectBoundBox a, ObjectBoundBox b) {
    return boxCompare(a.o, b.o, 0);
  }

  bool boxYCompare(ObjectBoundBox a, ObjectBoundBox b) {
    return boxCompare(a.o, b.o, 1);
  }

  bool boxZCompare(ObjectBoundBox a, ObjectBoundBox b) {
    return boxCompare(a.o, b.o, 2);
  }

  int findObjectSplitIndex(BvhItemBuild node, int axis) {
    float length = node.box.max[axis] - node.box.min[axis];
    float costArr[splitNumber];

    for (int i = 0; i < splitNumber; i++) {
      float leftLength = length * (i + 1) / (splitNumber + 1);
      int totalLeft = 0, totalRight = 0;

      for (auto &&item : node.objects) {
        Aabb curBox = objectBoundingBox(item.o);
        float pos = (curBox.max[axis] - curBox.min[axis]) / 2;

        if (pos < leftLength) {
          totalLeft++;
        } else {
          totalRight++;
        }
      }

      float probLeft = leftLength / length;
      float probRight = (length - leftLength) / length;

      costArr[i] = 0.5 + probLeft * totalLeft * 1 + probRight * totalRight * 1;
    }

    return std::distance(costArr, std::min_element(costArr, costArr + splitNumber));
  }

  // Since GPU can't deal with tree structures we need to create a flattened BVH.
  // Stack is used instead of a tree.
  std::vector<BvhNode> createBvh(const std::vector<ObjectBoundBox> &srcObjects) {
    int nodeCounter = 0;
    std::vector<BvhItemBuild> intermediate;
    std::stack<BvhItemBuild> nodeStack;

    BvhItemBuild root;
    root.index = nodeCounter;
    root.objects = srcObjects;

    nodeCounter++;
    nodeStack.push(root);

    while (!nodeStack.empty()) {
      BvhItemBuild currentNode = nodeStack.top();
      nodeStack.pop();

      currentNode.box = objectListBoundingBox(currentNode.objects);

      int axis = currentNode.box.longestAxis();
      auto comparator = (axis == 0) ? boxXCompare
                      : (axis == 1) ? boxYCompare
                      : boxZCompare;

      size_t objectSpan = currentNode.objects.size();
      std::sort(currentNode.objects.begin(), currentNode.objects.end(), comparator);

      if (objectSpan <= 2) {
        intermediate.push_back(currentNode);
        continue;
      } else {
        float length = currentNode.box.max[axis] - currentNode.box.min[axis];
        int mid = findObjectSplitIndex(currentNode, axis); // std::ceil(objectSpan / 2);

        float leftLength = length * (mid + 1) / (splitNumber + 1);

        BvhItemBuild leftNode;
        leftNode.index = nodeCounter;
        /* for (int i = 0; i < mid; i++) {
          leftNode.objects.push_back(currentNode.objects[i]);
        } */

        for (auto &&item : currentNode.objects) {
          Aabb curBox = objectBoundingBox(item.o);
          float pos = (curBox.max[axis] - curBox.min[axis]) / 2;

          if (pos < leftLength) {
            leftNode.objects.push_back(item);
          }
        }

        nodeCounter++;
        nodeStack.push(leftNode);

        BvhItemBuild rightNode;
        rightNode.index = nodeCounter;
        /* for (int i = mid; i < objectSpan; i++) {
          rightNode.objects.push_back(currentNode.objects[i]);
        } */

        for (auto &&item : currentNode.objects) {
          Aabb curBox = objectBoundingBox(item.o);
          float pos = (curBox.max[axis] - curBox.min[axis]) / 2;

          if (pos >= leftLength) {
            rightNode.objects.push_back(item);
          }
        }

        nodeCounter++;
        nodeStack.push(rightNode);

        currentNode.leftNodeIndex = leftNode.index;
        currentNode.rightNodeIndex = rightNode.index;
        intermediate.push_back(currentNode);
      }
    }

    std::sort(intermediate.begin(), intermediate.end(), nodeCompare);

    std::vector<BvhNode> output;
    // output.reserve(intermediate.size());

    for (int i = 0; i < intermediate.size(); i++) {
      output.emplace_back(intermediate[i].getGpuModel());
    }

    return output;
  }
}// namespace nugiEngine 
