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

vec3 setFaceNormal(vec3 r_direction, vec3 outwardNormal) {
  return dot(r_direction, outwardNormal) < 0.0 ? outwardNormal : -1.0 * outwardNormal;
}

vec2 getTotalTextureCoordinate(TextureCoordinate textCoord, vec2 uv) {
  float u = (1.0 - uv.x - uv.y) * textCoord.texel0.x + uv.x * textCoord.texel0.y + uv.y * textCoord.texel0.z;
  float v = (1.0 - uv.x - uv.y) * textCoord.texel1.x + uv.x * textCoord.texel1.y + uv.y * textCoord.texel1.z;

  return vec2(u, v);
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
  hit.point = (transformations[transformIndex].pointMatrix * vec4(rayAt(r, root), 1.0)).xyz; 

  vec3 outwardNormal = (hit.point - sphere.center) / sphere.radius;
  hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * setFaceNormal(r.direction, outwardNormal));
  
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
  hit.point = (transformations[transformIndex].pointMatrix * vec4(rayAt(r, t), 1.0)).xyz;
  hit.uv = vec2(u, v);

  vec3 outwardNormal = normalize(cross(v0v1, v0v2));
  hit.normal = normalize(mat3(transformations[transformIndex].normalMatrix) * setFaceNormal(r.direction, outwardNormal));

  return hit;
}

HitRecord hitLight(Triangle tri, Ray r, float tMin, float tMax) {
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
  hit.point = rayAt(r, t);
  hit.uv = vec2(u, v);

  vec3 outwardNormal = normalize(cross(v0v1, v0v2));
  hit.normal = setFaceNormal(r.direction, outwardNormal);

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

HitRecord hitPrimitiveBvh(Ray r, float tMin, float tMax, uint firstBvhIndex, uint firstPrimitiveIndex, int transformIndex) {
  HitRecord hit;
  hit.isHit = false;
  hit.t = tMax;

  uint stack[30];
  stack[0] = 1;

  int stackIndex = 1;  

  r.origin = (transformations[transformIndex].pointInverseMatrix * vec4(r.origin, 1.0)).xyz;
  r.direction = mat3(transformations[transformIndex].dirInverseMatrix) * r.direction;

  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].minimum, primitiveBvhNodes[currentNode - 1u + firstBvhIndex].maximum)) {
      continue;
    }

    uint primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].leftObjIndex;
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].triangle, r, tMin, hit.t, transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
        hit.uv = getTotalTextureCoordinate(primitives[hit.hitIndex].textCoord, hit.uv);
      }
    }

    primIndex = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].rightObjIndex;    
    if (primIndex >= 1u) {
      HitRecord tempHit = hitTriangle(primitives[primIndex - 1u + firstPrimitiveIndex].triangle, r, tMin, hit.t, transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = primIndex - 1u + firstPrimitiveIndex;
        hit.uv = getTotalTextureCoordinate(primitives[hit.hitIndex].textCoord, hit.uv);
      }
    }

    uint bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = primitiveBvhNodes[currentNode - 1u + firstBvhIndex].rightNode;
    if (bvhNode >= 1u) {
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

  uint stack[30];
  stack[0] = 1;

  int stackIndex = 1;
  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, objectBvhNodes[currentNode - 1u].minimum, objectBvhNodes[currentNode - 1u].maximum)) {
      continue;
    }

    uint objIndex = objectBvhNodes[currentNode - 1u].leftObjIndex;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, tMin, hit.t, objects[objIndex - 1u].firstBvhIndex, objects[objIndex - 1u].firstPrimitiveIndex, objects[objIndex - 1u].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    objIndex = objectBvhNodes[currentNode - 1u].rightObjIndex;
    if (objIndex >= 1u) {
      HitRecord tempHit = hitPrimitiveBvh(r, tMin, hit.t, objects[objIndex - 1u].firstBvhIndex, objects[objIndex - 1u].firstPrimitiveIndex, objects[objIndex - 1u].transformIndex);

      if (tempHit.isHit) {
        hit = tempHit;
      }
    }

    uint bvhNode = objectBvhNodes[currentNode - 1u].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = objectBvhNodes[currentNode - 1u].rightNode;
    if (bvhNode >= 1u) {
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
    HitRecord tempHit = hitLight(lights[i].triangle, r, tMin, hit.t);
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

  uint stack[30];
  stack[0] = 1;

  int stackIndex = 1;
  while(stackIndex > 0 && stackIndex <= 30) {
    stackIndex--;
    uint currentNode = stack[stackIndex];
    if (currentNode == 0u) {
      continue;
    }

    if (!intersectAABB(r, lightBvhNodes[currentNode - 1u].minimum, lightBvhNodes[currentNode - 1u].maximum)) {
      continue;
    }

    uint lightIndex = lightBvhNodes[currentNode - 1u].leftObjIndex;
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitLight(lights[lightIndex - 1u].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    lightIndex = lightBvhNodes[currentNode - 1u].rightObjIndex;    
    if (lightIndex >= 1u) {
      HitRecord tempHit = hitLight(lights[lightIndex - 1u].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.hitIndex = lightIndex - 1u;
      }
    }

    uint bvhNode = lightBvhNodes[currentNode - 1u].leftNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = lightBvhNodes[currentNode - 1u].rightNode;
    if (bvhNode >= 1u) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}