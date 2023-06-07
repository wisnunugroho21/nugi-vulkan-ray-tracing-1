#include "bvh.hpp"

namespace nugiEngine {
  int Aabb::longestAxis() {
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

  int Aabb::randomAxis() {
    return rand() % 3;
  }

  Aabb TriangleBoundBox::boundingBox() {
    return Aabb{ 
      glm::min(glm::min(this->triangles->point0, this->triangles->point1), this->triangles->point2) - glm::vec4(eps, 0.0f),
      glm::max(glm::max(this->triangles->point0, this->triangles->point1), this->triangles->point2) + glm::vec4(eps, 0.0f)
    };
  }

  Aabb SphereBoundBox::boundingBox() {
    return Aabb { 
      this->spheres->center - this->spheres->radius - glm::vec4(eps, 0.0f),
      this->spheres->center + this->spheres->radius + glm::vec4(eps, 0.0f)
    };
  }

  Aabb PrimitiveBoundBox::boundingBox() {
    return Aabb { 
      glm::min(glm::min(this->primitives->triangle.point0, this->primitives->triangle.point1), this->primitives->triangle.point2) - glm::vec4(eps, 0.0f),
      glm::max(glm::max(this->primitives->triangle.point0, this->primitives->triangle.point1), this->primitives->triangle.point2) + glm::vec4(eps, 0.0f)
    };
  }

  Aabb LightBoundBox::boundingBox() {
    return Aabb { 
      glm::min(glm::min(this->lights->triangle.point0, this->lights->triangle.point1), this->lights->triangle.point2) - glm::vec4(eps, 0.0f),
      glm::max(glm::max(this->lights->triangle.point0, this->lights->triangle.point1), this->lights->triangle.point2) + glm::vec4(eps, 0.0f)
    };
  }

  Aabb ObjectBoundBox::boundingBox() {
    auto min = glm::vec3(this->findMin(0), this->findMin(1), this->findMin(2)) - eps;
    auto max = glm::vec3(this->findMax(0), this->findMax(1), this->findMax(2)) + eps;

    auto rotationMat = glm::mat3(this->transformation->rotationMatrix());
    auto scaleVec = glm::vec3(this->transformation->scale);
    auto transVec = glm::vec3(this->transformation->translation);    

    return Aabb { 
        glm::vec4(rotationMat * (min * scaleVec) + transVec, 1.0f),
        glm::vec4(rotationMat * (max * scaleVec) + transVec, 1.0f),
    };
  }

  float ObjectBoundBox::findMax(uint32_t index) {
    float max = -999.0f;
    for (auto &&primitive : this->primitives) {
      if (primitive->triangle.point0[index] > max) max = primitive->triangle.point0[index];
      if (primitive->triangle.point1[index] > max) max = primitive->triangle.point1[index];
      if (primitive->triangle.point2[index] > max) max = primitive->triangle.point2[index];
    }

    return max;
  }

  float ObjectBoundBox::findMin(uint32_t index) {
    float min = 999.0f;
    for (auto &&primitive : this->primitives) {
      if (primitive->triangle.point0[index] < min) min = primitive->triangle.point0[index];
      if (primitive->triangle.point1[index] < min) min = primitive->triangle.point1[index];
      if (primitive->triangle.point2[index] < min) min = primitive->triangle.point2[index];
    }

    return min;
  }

  BvhNode BvhItemBuild::getGpuModel() {
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

  bool boxCompare(std::shared_ptr<BoundBox> a, std::shared_ptr<BoundBox> b, int axis) {
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

  int findPrimitiveSplitIndex(BvhItemBuild node, int axis, float length) {
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
  std::vector<std::shared_ptr<BvhNode>> createBvh(const std::vector<std::shared_ptr<BoundBox>> boundedBoxes) {
    int nodeCounter = 0;
    std::vector<BvhItemBuild> intermediate;
    std::stack<BvhItemBuild> nodeStack;

    BvhItemBuild root;
    root.index = nodeCounter;
    root.objects = boundedBoxes;

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
        int mid = findPrimitiveSplitIndex(currentNode, axis, length); //  std::ceil(objectSpan / 2);

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
    std::vector<std::shared_ptr<BvhNode>> output;

    for (int i = 0; i < intermediate.size(); i++) {
      auto node = std::make_shared<BvhNode>(intermediate[i].getGpuModel());
      output.emplace_back(node);
    }

    return output;
  }
}