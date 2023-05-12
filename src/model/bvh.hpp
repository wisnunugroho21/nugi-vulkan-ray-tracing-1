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
  const int splitNumber = 11;

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
  struct BoundBox {
    int index;

    BoundBox(int i) : index{i} {}

    virtual Aabb boundingBox() = 0;
  };

  struct TriangleBoundBox : BoundBox {
    Triangle t;

    TriangleBoundBox(int i, Triangle t) : BoundBox(i), t{t} {}

    Aabb boundingBox() {
      return Aabb{ 
        glm::min(glm::min(this->t.point0, this->t.point1), this->t.point2) - eps, 
        glm::max(glm::max(this->t.point0, this->t.point1), this->t.point2) + eps 
      };
    }
  };

  struct SphereBoundBox : BoundBox {
    Sphere s;

    SphereBoundBox(int i, Sphere s) : BoundBox(i), s{s} {}

    Aabb boundingBox() {
      return Aabb { 
        this->s.center - this->s.radius - eps, 
        this->s.center + this->s.radius + eps 
      };
    }
  };

  struct ObjectBoundBox : BoundBox {
    Object o;

    ObjectBoundBox(int i, Object o) : BoundBox(i), o{o} {}

    Aabb boundingBox() {
      return Aabb { 
        glm::min(glm::min(this->o.triangle.point0, this->o.triangle.point1), this->o.triangle.point2) - eps, 
        glm::max(glm::max(this->o.triangle.point0, this->o.triangle.point1), this->o.triangle.point2) + eps 
      };
    }
  };

  // Intermediate BvhNode structure needed for constructing Bvh.
  struct BvhItemBuild {
    Aabb box;
    int index = -1; // index refers to the index in the final array of nodes. Used for sorting a flattened Bvh.
    int leftNodeIndex = -1;
    int rightNodeIndex = -1;
    std::vector<std::shared_ptr<BoundBox>> objects;

    BvhNode getGpuModel() {
      bool leaf = leftNodeIndex == -1 && rightNodeIndex == -1;

      BvhNode node{};
      node.minimum = box.min;
      node.maximum = box.max;      

      if (leaf) {
        node.leftObjIndex = objects[0]->index;

        if (objects.size() > 1) {
          node.rightObjIndex = objects[1]->index;
        }
      } else {
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

  Aabb objectListBoundingBox(std::vector<std::shared_ptr<BoundBox>> &objects) {
    Aabb tempBox;
    Aabb outputBox;
    bool firstBox = true;

    for (auto &object : objects) {
      tempBox = object->boundingBox();
      outputBox = firstBox ? tempBox : surroundingBox(outputBox, tempBox);
      firstBox = false;
    }

    return outputBox;
  }

  inline bool boxCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b, int axis) {
    Aabb boxA = a->boundingBox();
    Aabb boxB = b->boundingBox();

    float Apos = (boxA.max[axis] - boxA.min[axis]) / 2 + boxA.min[axis];
    float Bpos = (boxB.max[axis] - boxB.min[axis]) / 2 + boxB.min[axis];

    return Apos < Bpos;
  }

  bool boxXCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b) {
    return boxCompare(a, b, 0);
  }

  bool boxYCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b) {
    return boxCompare(a, b, 1);
  }

  bool boxZCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b) {
    return boxCompare(a, b, 2);
  }

  int findObjectSplitIndex(BvhItemBuild node, int axis, float length) {
    float costArr[splitNumber]{};

    for (int i = 0; i < splitNumber; i++) {
      int totalLeft = 0, totalRight = 0;

      float leftLength = length * (i + 1) / (splitNumber + 1);
      float posBarrier = leftLength + node.box.min[axis];

      for (auto &&item : node.objects) {
        Aabb curBox = item->boundingBox();
        float pos = (curBox.max[axis] - curBox.min[axis]) / 2 + curBox.min[axis];

        if (pos < posBarrier) {
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
  std::vector<BvhNode> createBvh(const std::vector<std::shared_ptr<BoundBox>> srcObjects) {
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
        int mid = findObjectSplitIndex(currentNode, axis, length); // std::ceil(objectSpan / 2);

        float posBarrier = length * (mid + 1) / (splitNumber + 1) + currentNode.box.min[axis];
        BvhItemBuild leftNode, rightNode;

        for (auto &&item : currentNode.objects) {
          Aabb curBox = item->boundingBox();
          float pos = (curBox.max[axis] - curBox.min[axis]) / 2 + curBox.min[axis];

          if (pos < posBarrier) {
            leftNode.objects.push_back(item);
          } else {
            rightNode.objects.push_back(item);
          }
        }

        if (leftNode.objects.size() == 0 || rightNode.objects.size() == 0) {
					mid = std::ceil(objectSpan / 2);

					leftNode.objects.clear();
					rightNode.objects.clear();

					for (int i = 0; i < mid; i++) {
						leftNode.objects.push_back(currentNode.objects[i]);
					}

					for (int i = mid; i < objectSpan; i++) {
						rightNode.objects.push_back(currentNode.objects[i]);
					}
        }        

        leftNode.index = nodeCounter;
        nodeCounter++;
        nodeStack.push(leftNode);

        rightNode.index = nodeCounter;
        nodeCounter++;
        nodeStack.push(rightNode);

        currentNode.leftNodeIndex = leftNode.index;
        currentNode.rightNodeIndex = rightNode.index;
        intermediate.push_back(currentNode);
      }
    }

    std::sort(intermediate.begin(), intermediate.end(), nodeCompare);
    std::vector<BvhNode> output;

    for (int i = 0; i < intermediate.size(); i++) {
      output.emplace_back(intermediate[i].getGpuModel());
    }

    return output;
  }
}// namespace nugiEngine 
