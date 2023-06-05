// ------------- Basic -------------

vec3 rayAt(Ray r, float t) {
  return r.origin + t * r.direction;
}

vec3[3] buildOnb(vec3 normal) {
  vec3 a = abs(normalize(normal).x) > 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);

  vec3 z = normalize(normal);
  vec3 y = normalize(cross(z, a));
  vec3 x = cross(z, y);

  return vec3[3](x, y, z);
}

FaceNormal setFaceNormal(vec3 r_direction, vec3 outwardNormal) {
  FaceNormal faceNormal;

  faceNormal.frontFace = dot(r_direction, outwardNormal) < 0.0;
  faceNormal.normal = faceNormal.frontFace ? outwardNormal : -1.0 * outwardNormal;

  return faceNormal;
}

// ------------- Sphere -------------

HitRecord hitSphere(Sphere sphere, Ray r, float tMin, float tMax, int transformIndex) {
  HitRecord hit;
  hit.isHit = false;

  vec3 oc = r.origin - sphere.center;

  float a = dot(r.direction, r.direction);
  float half_b = dot(oc, r.direction);
  float c = dot(oc, oc) - sphere.radius * sphere.radius;

  float discriminant = half_b * half_b - a * c;

  if (discriminant < 0.0) {
    return hit;
  }

  float sqrtd = sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  float root = (-half_b - sqrtd) / a;
  if (root < tMin || root > tMax) {
    root = (-half_b + sqrtd) / a;
    if (root < tMin || root > tMax) {
      return hit;
    }
  }

  hit.isHit = true;
  hit.t = root;
  hit.point = transformations[transformIndex].rotationMatrix * (rayAt(r, root) * transformations[transformIndex].scalingVector + transformations[transformIndex].translationVector);

  vec3 outwardNormal = (hit.point - sphere.center) / sphere.radius;
  hit.faceNormal = setFaceNormal(r.direction, normalize(transformations[transformIndex].rotationMatrix * (outwardNormal / transformations[transformIndex].scalingVector)));
  
  return hit;
}

// ------------- Triangle -------------

HitRecord hitTriangle(Triangle tri, Ray r, float tMin, float tMax, int transformIndex) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = tri.point1 - tri.point0;
  vec3 v0v2 = tri.point2 - tri.point0;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }
    
  float invDet = 1.0 / det;

  vec3 tvec = r.origin - tri.point0;
  float u = dot(tvec, pvec) * invDet;
  if (u < 0.0 || u > 1.0) {
    return hit;
  }

  vec3 qvec = cross(tvec, v0v1);
  float v = dot(r.direction, qvec) * invDet;
  if (v < 0.0 || u + v > 1.0) {
    return hit;
  }
  
  float t = dot(v0v2, qvec) * invDet;

  if (t <= KEPSILON) {
    return hit;
  }

  if (t < tMin || t > tMax) {
    return hit;
  }

  hit.isHit = true;
  hit.t = t;
  hit.point = transformations[transformIndex].rotationMatrix * (rayAt(r, t) * transformations[transformIndex].scalingVector + transformations[transformIndex].translationVector);
  hit.uv = vec2(u, v);

  vec3 outwardNormal = normalize(cross(v0v1, v0v2));
  hit.faceNormal = setFaceNormal(r.direction, normalize(transformations[transformIndex].rotationMatrix * (outwardNormal / transformations[transformIndex].scalingVector)));

  return hit;
}

// ------------- Bvh -------------

bool intersectAABB(Ray r, vec3 boxMin, vec3 boxMax) {
  if (all(lessThan(boxMin, r.origin)) && all(greaterThan(boxMax, r.origin))) {
    return true;
  }

  vec3 tMin = (boxMin - r.origin) / r.direction;
  vec3 tMax = (boxMax - r.origin) / r.direction;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);

  return tNear < tFar;
}

HitRecord hitPrimitiveBvh(Ray r, float tMin, float tMax, int firstBvhIndex, int firstPrimitiveIndex, int transformIndex) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  int stack[30];
  int stackIndex = 0;

  stack[0] = 0;
  stackIndex++;

  r.origin = transformations[transformIndex].rotationMatrix * ((r.origin - transformations[transformIndex].translationVector) / transformations[transformIndex].scalingVector);
  r.direction = transformations[transformIndex].rotationMatrix * (r.direction / transformations[transformIndex].scalingVector);

  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    int currentNode = stack[stackIndex];
    if (currentNode < 0) {
      continue;
    }

    if (!intersectAABB(r, primitiveBvhNodes[currentNode + firstBvhIndex].minimum, primitiveBvhNodes[currentNode + firstBvhIndex].maximum)) {
      continue;
    }

    int primIndex = primitiveBvhNodes[currentNode + firstBvhIndex].leftObjIndex;
    if (primIndex >= 0) {
      HitRecord tempHit = hitTriangle(primitives[primIndex + firstPrimitiveIndex].triangle, r, tMin, hit.t, transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex + firstPrimitiveIndex;
      }
    }

    primIndex = primitiveBvhNodes[currentNode + firstBvhIndex].rightObjIndex;    
    if (primIndex >= 0) {
      HitRecord tempHit = hitTriangle(primitives[primIndex + firstPrimitiveIndex].triangle, r, tMin, hit.t, transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex + firstPrimitiveIndex;
      }
    }

    int bvhNode = primitiveBvhNodes[currentNode + firstBvhIndex].leftNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = primitiveBvhNodes[currentNode + firstBvhIndex].rightNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}

HitRecord hitObjectBvh(Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  int stack[30];
  int stackIndex = 0;

  stack[0] = 0;
  stackIndex++;

  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    int currentNode = stack[stackIndex];
    if (currentNode < 0) {
      continue;
    }

    if (!intersectAABB(r, objectBvhNodes[currentNode].minimum, objectBvhNodes[currentNode].maximum)) {
      continue;
    }

    int objIndex = objectBvhNodes[currentNode].leftObjIndex;
    if (objIndex >= 0) {
      HitRecord tempHit = hitPrimitiveBvh(r, tMin, hit.t, objects[objIndex].firstBvhIndex, objects[objIndex].firstPrimitiveIndex, objects[objIndex].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    objIndex = objectBvhNodes[currentNode].rightObjIndex;
    if (objIndex >= 0) {
      HitRecord tempHit = hitPrimitiveBvh(r, tMin, hit.t, objects[objIndex].firstBvhIndex, objects[objIndex].firstPrimitiveIndex, objects[objIndex].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    int bvhNode = objectBvhNodes[currentNode].leftNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = objectBvhNodes[currentNode].rightNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}

// ------------- Light -------------

HitRecord hitLightList(Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  for (int i = 0; i < 2; i++) {
    HitRecord tempHit = hitTriangle(lights[i].triangle, r, tMin, hit.t, 0);
    if (tempHit.isHit) {
      hit = tempHit;
      hit.hitIndex = i;
    }
  }

  return hit;
}

HitRecord hitLightBvh(Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  int stack[30];
  int stackIndex = 0;

  stack[0] = 0;
  stackIndex++;

  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    int currentNode = stack[stackIndex];
    if (currentNode < 0) {
      continue;
    }

    if (!intersectAABB(r, lightBvhNodes[currentNode].minimum, lightBvhNodes[currentNode].maximum)) {
      continue;
    }

    int lightIndex = lightBvhNodes[currentNode].leftObjIndex;
    if (lightIndex >= 0) {
      HitRecord tempHit = hitTriangle(lights[lightIndex].triangle, r, tMin, hit.t, 0);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex;
      }
    }

    lightIndex = lightBvhNodes[currentNode].rightObjIndex;    
    if (lightIndex >= 0) {
      HitRecord tempHit = hitTriangle(lights[lightIndex].triangle, r, tMin, hit.t, 0);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex;
      }
    }

    int bvhNode = lightBvhNodes[currentNode].leftNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = lightBvhNodes[currentNode].rightNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}