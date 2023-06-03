// ------------- Basic -------------

vec4 rayAt(Ray r, float t) {
  return r.origin + t * r.direction;
}

vec4[3] buildOnb(vec4 normal) {
  vec4 a = abs(normalize(normal).x) > 0.9 ? vec4(0.0, 1.0, 0.0, 1.0) : vec4(1.0, 0.0, 0.0, 1.0);

  vec4 z = normalize(normal);
  vec4 y = vec4(normalize(cross(z.xyz, a.xyz)), 1.0);
  vec4 x = vec4(cross(z.xyz, y.xyz), 1.0);

  return vec4[3](x, y, z);
}

FaceNormal setFaceNormal(vec4 r_direction, vec4 outwardNormal) {
  FaceNormal faceNormal;

  faceNormal.frontFace = dot(r_direction, outwardNormal) < 0.0;
  faceNormal.normal = faceNormal.frontFace ? outwardNormal : -1.0 * outwardNormal;

  return faceNormal;
}

// ------------- Sphere -------------

HitRecord hitSphere(Sphere sphere, Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec4 oc = r.origin - sphere.center;

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
  hit.point = rayAt(r, hit.t);

  vec4 outwardNormal = (hit.point - sphere.center) / sphere.radius;
  hit.faceNormal = setFaceNormal(r.direction, outwardNormal);
  
  return hit;
}

// ------------- Triangle -------------

HitRecord hitTriangle(Triangle tri, Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec4 v0v1 = tri.point1 - tri.point0;
  vec4 v0v2 = tri.point2 - tri.point0;
  vec4 pvec = vec4(cross(r.direction.xyz, v0v2.xyz), 1.0);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }
    
  float invDet = 1.0 / det;

  vec4 tvec = r.origin - tri.point0;
  float u = dot(tvec, pvec) * invDet;
  if (u < 0.0 || u > 1.0) {
    return hit;
  }

  vec4 qvec = vec4(cross(tvec.xyz, v0v1.xyz), 1.0);
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
  hit.point = rayAt(r, t);
  hit.uv = vec2(u, v);

  vec4 outwardNormal = vec4(normalize(cross(v0v1.xyz, v0v2.xyz)), 1.0);
  hit.faceNormal = setFaceNormal(r.direction, outwardNormal);

  return hit;
}

// ------------- Bvh -------------

bool intersectAABB(Ray r, vec4 boxMin, vec4 boxMax) {
  if (all(lessThan(boxMin, r.origin)) && all(greaterThan(boxMax, r.origin))) {
    return true;
  }

  vec4 tMin = (boxMin - r.origin) / r.direction;
  vec4 tMax = (boxMax - r.origin) / r.direction;
  vec4 t1 = min(tMin, tMax);
  vec4 t2 = max(tMin, tMax);
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
      HitRecord tempHit = hitTriangle(primitives[primIndex + firstPrimitiveIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitPrimIndex = primIndex + firstPrimitiveIndex;
      }
    }

    primIndex = primitiveBvhNodes[currentNode + firstBvhIndex].rightObjIndex;    
    if (primIndex >= 0) {
      HitRecord tempHit = hitTriangle(primitives[primIndex + firstPrimitiveIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitPrimIndex = primIndex + firstPrimitiveIndex;
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
        hit.hitObjIndex = objIndex;
      }
    }

    objIndex = objectBvhNodes[currentNode].rightObjIndex;
    if (objIndex >= 0) {
      HitRecord tempHit = hitPrimitiveBvh(r, tMin, hit.t, objects[objIndex].firstBvhIndex, objects[objIndex].firstPrimitiveIndex, objects[objIndex].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitObjIndex = objIndex;
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
    HitRecord tempHit = hitTriangle(lights[i].triangle, r, tMin, hit.t);
    if (tempHit.isHit) {
      hit = tempHit;
      hit.hitPrimIndex = i;
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
      HitRecord tempHit = hitTriangle(lights[lightIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitPrimIndex = lightIndex;
      }
    }

    lightIndex = lightBvhNodes[currentNode].rightObjIndex;    
    if (lightIndex >= 0) {
      HitRecord tempHit = hitTriangle(lights[lightIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitPrimIndex = lightIndex;
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