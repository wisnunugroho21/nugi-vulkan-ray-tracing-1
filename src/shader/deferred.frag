#version 460

// ------------- layout -------------

#define KEPSILON 0.00001

struct Sphere {
  vec3 center;
  float radius;
};

struct Triangle {
  vec3 point0;
  vec3 point1;
  vec3 point2;
};

struct Model {
  Triangle triangle;
  vec3 normal;
  uint materialIndex;
};

struct PointLight {
	Sphere sphere;
  vec3 color;
};

struct BvhNode {
  int leftNode;
  int rightNode;
  int leftObjIndex;
  int rightObjIndex;

  vec3 maximum;
  vec3 minimum;
};

layout(origin_upper_left) in vec4 gl_FragCoord;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

layout(set = 0, binding = 2) buffer readonly GlobalModel {
  Model models[100];
  uint numModel;
} globalModel;

layout(set = 0, binding = 3) buffer readonly GlobalBvh {
  BvhNode bvhNodes[100];
  uint numNode;
} globalBvh;

layout(set = 1, binding = 0, rgba8) uniform readonly image2D positionResource;
layout(set = 1, binding = 1, rgba8) uniform readonly image2D albedoResource;
layout(set = 1, binding = 2, rgba8) uniform readonly image2D normalResource;

layout(push_constant) uniform Push {
  uint randomSeed;
} push;

// ------------- pre-defined parameter -------------

float pi = 3.14159265359;

// ------------- struct ------------- 

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct FaceNormal {
  bool frontFace;
  vec3 normal;
};

struct HitRecord {
  bool isHit;
  uint objIndex;

  float t;
  vec3 point;
  vec2 uv;

  FaceNormal faceNormal;
};

struct ShadeRecord {
  vec3 colorAttenuation;
  vec3 colorEmitted;

  Ray raySpecular;
};

// ------------- function ------------- 

#include "helper/random.glsl"

// Return true if the vector is close to zero in all dimensions.
bool nearZero(vec3 xyz) {
  return (abs(xyz.x) < 1e-8) && (abs(xyz.y) < 1e-8) && (abs(xyz.z) < 1e-8);
}

vec3 rayAt(Ray r, float t) {
  return r.origin + t * r.direction;
}

FaceNormal setFaceNormal(vec3 r_direction, vec3 outwardNormal) {
  FaceNormal faceNormal;

  faceNormal.frontFace = dot(r_direction, outwardNormal) < 0.0;
  faceNormal.normal = faceNormal.frontFace ? outwardNormal : -1.0 * outwardNormal;

  return faceNormal;
}

// ------------- Hit Shape -------------

HitRecord hitSphere(Sphere obj, Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec3 oc = r.origin - obj.center;

  float a = dot(r.direction, r.direction);
  float half_b = dot(oc, r.direction);
  float c = dot(oc, oc) - obj.radius * obj.radius;

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

  vec3 outwardNormal = (hit.point - obj.center) / obj.radius;
  hit.faceNormal = setFaceNormal(r.direction, outwardNormal);
  
  return hit;
}

HitRecord hitTriangle(Triangle obj, Ray r, float tMin, float tMax) {
  HitRecord hit;
  hit.isHit = false;

  vec3 v0v1 = obj.point1 - obj.point0;
  vec3 v0v2 = obj.point2 - obj.point0;
  vec3 pvec = cross(r.direction, v0v2);
  float det = dot(v0v1, pvec);
  
  if (abs(det) < KEPSILON) {
    return hit;
  }
    
  float invDet = 1.0 / det;

  vec3 tvec = r.origin - obj.point0;
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
  hit.faceNormal = setFaceNormal(r.direction, outwardNormal);

  return hit;
}

bool intersectAABB(Ray r, vec3 boxMin, vec3 boxMax) {
  vec3 tMin = (boxMin - r.origin) / r.direction;
  vec3 tMax = (boxMax - r.origin) / r.direction;
  vec3 t1 = min(tMin, tMax);
  vec3 t2 = max(tMin, tMax);
  float tNear = max(max(t1.x, t1.y), t1.z);
  float tFar = min(min(t2.x, t2.y), t2.z);

  return tNear < tFar;
}

HitRecord hitBvh(Ray r, float tMin, float tMax) {
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

    if (!intersectAABB(r, globalBvh.bvhNodes[currentNode].minimum, globalBvh.bvhNodes[currentNode].maximum)) {
      continue;
    }

    int objIndex = globalBvh.bvhNodes[currentNode].leftObjIndex;
    if (objIndex >= 0) {
      HitRecord tempHit = hitTriangle(globalModel.models[objIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.objIndex = objIndex;
      }
    }

    objIndex = globalBvh.bvhNodes[currentNode].rightObjIndex;    
    if (objIndex >= 0) {
      HitRecord tempHit = hitTriangle(globalModel.models[objIndex].triangle, r, tMin, hit.t);

      if (tempHit.isHit) {
        hit = tempHit;
        hit.objIndex = objIndex;
      }
    }

    int bvhNode = globalBvh.bvhNodes[currentNode].leftNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }

    bvhNode = globalBvh.bvhNodes[currentNode].rightNode;
    if (bvhNode >= 0) {
      stack[stackIndex] = bvhNode;
      stackIndex++;
    }
  }

  return hit;
}

// ------------- GGX -------------

float fresnelSchlick(float VoH, float F0) {
  return F0 + (1.0 - F0) * pow(1.0 - VoH, 5.0);
} 

float D_GGX(float NoH, float roughness) {
  float r = max(roughness, 0.05);
  
  float alpha = r * r;
  float alpha2 = alpha * alpha;
  
  float b = (NoH * NoH * (alpha2 - 1.0) + 1.0);
  return alpha2 / (pi * b * b);
}
float G1_GGX_Schlick(float cosine, float roughness) {
  float r = 0.5 + 0.5 * roughness; // Disney remapping
  float k = (r * r) / 2.0;

  float denom = cosine * (1.0 - k) + k;
  return cosine / denom;
}

float G1_GGX(float cosine, float roughness) {
  float alpha = roughness * roughness;
  float alpha2 = alpha * alpha;

  float b = alpha2 + (1 - alpha2) * cosine * cosine;
  return 2 * cosine / (cosine + sqrt(b));
}

float G_Smith(float NoV, float NoL, float roughness) {
  float g1_l = G1_GGX(NoL, roughness);
  float g1_v = G1_GGX(NoV, roughness);

  return g1_l * g1_v;
}

// ------------- Material -------------
float brdfVal(float NoV, float NoL, float NoH, float VoH) {
  float roughness = 0.0;
  float fresnelReflect = 0.2;
  float metallicness = 0.8;

  float f0 = 0.16 * (fresnelReflect * fresnelReflect); // F0 for dielectics in range [0.0, 0.16].  default FO is (0.16 * 0.5^2) = 0.04

  // specular microfacet (cook-torrance) BRDF
  float F = fresnelSchlick(VoH, f0);
  float D = D_GGX(NoH, roughness);
  float G = G_Smith(NoV, NoL, roughness);
  float spec = (F * D * G) / (4.0 * NoV * NoL);
  
  // diffuse
  float diff = 1.0 / pi;

  return mix(diff, spec, metallicness);
}

// ------------- Light -------------

vec3[3] buildOnb(vec3 normal) {
  vec3 a = abs(normalize(normal).x) > 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);

  vec3 z = normalize(normal);
  vec3 y = normalize(cross(z, a));
  vec3 x = cross(z, y);

  return vec3[3](x, y, z);
}

float areaLight(Sphere sphere) {
  return 4 * pi * sphere.radius * sphere.radius; 
}

vec3 sphereGenerateRandom(Sphere sphere, vec3 origin) {
  vec3 dist = sphere.center - origin;
  float distSqr = dot(dist, dist);

  vec3[3] globalOnb = buildOnb(dist);

  float r1 = randomFloat(0);
  float r2 = randomFloat(1);
  float z = 1 + r2 * (sqrt(1 - sphere.radius * sphere.radius / distSqr) - 1);

  float phi = 2 * pi * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  float y = sin(phi) * sqrt(1 - z * z);

  return x * globalOnb[0] + y * globalOnb[1] + z * globalOnb[2];
}

// ------------- Main -------------

void main() {
  vec3 fragPosWorld = imageLoad(positionResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragColor = imageLoad(albedoResource, ivec2(gl_FragCoord.xy)).xyz;
  vec3 fragNormalWorld = imageLoad(normalResource, ivec2(gl_FragCoord.xy)).xyz;

  vec3 totalColor = vec3(0.0);

  for (int i = 0; i < globalLight.numLight; i++) {
    vec3 viewDirection = normalize(ubo.realCameraPos - fragPosWorld);
    vec3 lightDirection = normalize(globalLight.pointLights[i].sphere.center - fragPosWorld);
    vec3 halAngle = normalize(lightDirection + viewDirection); // half vector

    float NoV = clamp(dot(fragNormalWorld, viewDirection), 0.001, 1.0);
    float NoL = clamp(dot(fragNormalWorld, lightDirection), 0.001, 1.0);
    float NoH = clamp(dot(fragNormalWorld, halAngle), 0.001, 1.0);
    float VoH = clamp(dot(viewDirection, halAngle), 0.001, 1.0);

    Ray r;
    r.origin = fragPosWorld;
    r.direction = lightDirection;

    HitRecord hitLight = hitSphere(globalLight.pointLights[i].sphere, r, 0.001, 1000000.0); 
    HitRecord hitObject = hitBvh(r, 0.001, (hitLight.isHit) ? hitLight.t : 1000000.0);

    if (hitObject.isHit) {
      continue;
    }

    vec3 distance = (hitLight.point - r.origin) / 10.0;
    float brdf = brdfVal(NoV, NoL, NoH, VoH);
    float NloL = dot(hitLight.faceNormal.normal, -1.0 * normalize(r.direction));

    totalColor += areaLight(globalLight.pointLights[i].sphere) * globalLight.pointLights[i].color * brdf * NoL * NloL / dot(distance, distance);
  }

  outColor = vec4(totalColor * fragColor, 1.0);
}