// ------------- GGX Function -------------

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

// ------------- Sphere Random ------------- 

float areaSphere(Sphere obj) {
  return 4 * pi * obj.radius * obj.radius;
}

float spherePdfValue(Sphere sphere, Ray r) {
  HitRecord hit = hitSphere(sphere, r, 0.001, 1000000.0);
  if (!hit.isHit) {
    return 0.0;
  }

  vec3 dist = sphere.center - r.origin;

  float cosThetaMax = sqrt(1 - sphere.radius * sphere.radius / dot(dist, dist));
  float solidAngle = 2 * pi * (1 - cosThetaMax);

  return  1.0 / solidAngle;
}

vec3 sphereGenerateRandom(Sphere sphere, vec3 origin) {
  vec3 dist = sphere.center - origin;
  vec3[3] globalOnb = buildOnb(dist);

  float r1 = randomFloat(0);
  float r2 = randomFloat(1);
  float z = 1 + r2 * (sqrt(1 - sphere.radius * sphere.radius /  dot(dist, dist)) - 1);

  float phi = 2 * pi * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  float y = sin(phi) * sqrt(1 - z * z);

  return x * globalOnb[0] + y * globalOnb[1] + z * globalOnb[2];
}

float sphereListPdfValue(Ray r) {
  float weight = 1.0 / 1;
  float sum = 0.0;

  for (int i = 0; i < 1; i++) {
    sum += weight * spherePdfValue(globalLight.pointLights[i].sphere, r);
  }

  return sum;
}

vec3 sphereListGenerateRandom(vec3 origin) {
  // return sphereGenerateRandom(globalLight.pointLights[randomInt(0, 1, 1)].sphere, origin);
  return sphereGenerateRandom(globalLight.pointLights[0].sphere, origin);
}

// ------------- Triangle Random -------------

float areaTriangle(Triangle obj) {
  vec3 v0v1 = obj.point1 - obj.point0;
  vec3 v0v2 = obj.point2 - obj.point0;

  vec3 pvec = cross(v0v1, v0v2);
  return 0.5 * sqrt(dot(pvec, pvec)); 
}

float trianglePdfValue(Triangle obj, Ray r) {
  HitRecord hit = hitTriangle(obj, r, 0.001, 1000.0);
  if (!hit.isHit) {
    return 0.0;
  }

  float cosine = abs(dot(r.direction, hit.faceNormal.normal) / length(r.direction));
  float distanceSquared = hit.t * hit.t * dot(r.direction, r.direction);
  float area = areaTriangle(obj);

  return distanceSquared / (cosine * area);
}

vec3 triangleGenerateRandom(Triangle obj, vec3 origin) {
  vec3 a = obj.point1 - obj.point0;
  vec3 b = obj.point2 - obj.point0;

  float u1 = randomFloat(1);
  float u2 = randomFloat(2);

  if (u1 + u2 > 1) {
    u1 = 1 - u1;
    u2 = 1 - u2;
  }

  vec3 randomTriangle = u1 * a + u2 * b + obj.point0;
  return randomTriangle - origin;
}

// ------------- GGX Random -------------

vec3 randomGGX(uint index1, uint index2, float roughness) {
  float r1 = randomFloat(index1);
  float r2 = randomFloat(index2);

  float a = roughness * roughness;
  float phi = 2 * 3.14159265359 * r2;

  float cosTheta = sqrt((1.0 - r1) / ((a * a - 1.0) * r1 + 1.0));
  float sinTheta = sqrt(1 - cosTheta * cosTheta);

  float x = cos(phi) * sinTheta;
  float y = sin(phi) * sinTheta;
  float z = cosTheta;

  return vec3(x, y, z);
}

float ggxPdfValue(float NoH, float NoL, float roughness) {
  return D_GGX(NoH, roughness) * NoH / (4.0 * NoL);
}

vec3 ggxGenerateRandom(vec3[3] globalOnb, float roughness) {
  vec3 source = randomGGX(0, 1, roughness);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

// ------------- Lambert Random ------------- 

vec3 randomCosineDirection(uint index1, uint index2) {
  float r1 = randomFloat(index1);
  float r2 = randomFloat(index2);
  float phi = 2 * 3.14159265359 * r1;
  
  float x = cos(phi) * sqrt(r2);
  float y = sin(phi) * sqrt(r2);
  float z = sqrt(1 - r2);

  return vec3(x, y, z);
}

float cosinePdfValue(vec3 normal, vec3 direction) {
  float cosine = dot(normalize(direction), normal);
  return max(cosine, 0.0001) / pi;
}

vec3 cosineGenerateRandom(vec3[3] globalOnb) {
  vec3 source = randomCosineDirection(0, 1);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}