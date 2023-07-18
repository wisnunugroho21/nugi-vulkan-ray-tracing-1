// ------------- GGX -------------

vec3 randomGGX(float roughness, uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed + 1);

  float a = roughness * roughness;
  float phi = 2 * 3.14159265359 * r2;

  float cosTheta = sqrt((1.0f - r1) / ((a * a - 1.0f) * r1 + 1.0f));
  float sinTheta = sqrt(1 - cosTheta * cosTheta);

  float x = cos(phi) * sinTheta;
  float y = sin(phi) * sinTheta;
  float z = cosTheta;

  return vec3(x, y, z);
}

vec3 ggxRandomDirection(vec3 viewDirection, vec3 normal, float roughness, uint additionalRandomSeed) {
  vec3[3] globalOnb = buildOnb(reflect(viewDirection, normal));
  vec3 source = randomGGX(roughness, additionalRandomSeed);

  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float ggxPdfValue(float NoH, float NoL, float roughness) {
  return D_GGX(NoH, roughness) * NoH / (4.0 * NoL);
}

float ggxBrdfValue(float NoV, float NoL, float NoH, float VoH, float f0, float roughness) {
  float F = fresnelSchlick(VoH, f0);
  float D = D_GGX(NoH, roughness);
  float G = G_Smith(NoV, NoL, roughness);

  return (F * D * G) / (4.0 * NoV * NoL);
}

ShadeRecord indirectGgxShade(vec3 rayDirection, vec3 point, vec3 normal, vec3 surfaceColor, float roughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  scat.nextRay.origin = point;

  vec3 unitViewDirection = normalize(rayDirection);
  float f0 = 0.16 * (fresnelReflect * fresnelReflect);

  scat.nextRay.direction = ggxRandomDirection(unitViewDirection, normal, roughness, additionalRandomSeed);
  vec3 H = normalize(scat.nextRay.direction - rayDirection); // half vector

  float NoL = max(dot(normal, normalize(scat.nextRay.direction)), 0.001f);
  float NoV = max(dot(normal, -1.0f * unitViewDirection), 0.001f);
  float NoH = max(dot(normal, H), 0.001f);
  float VoH = max(dot(unitViewDirection, H), 0.001f);

  float brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, f0, roughness);

  scat.pdf = ggxPdfValue(NoH, NoL, roughness);
  scat.radiance = integrandOverHemisphere(surfaceColor, brdf, NoL, scat.pdf);
  
  return scat;
}

ShadeRecord indirectGgxShade(Ray r, HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return indirectGgxShade(r.direction, hit.point, hit.normal, materials[materialIndex].baseColor, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, additionalRandomSeed);
}

ShadeRecord directGgxShade(vec3 rayDirection, vec3 point, vec3 normal, vec3 surfaceColor, float roughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.0f;

  shadowRay.origin = point;
  uint lightIndex = randomUint(0, ubo.numLights - 1u, additionalRandomSeed);

  shadowRay.direction = areaLightGenerateRandom(lights[lightIndex], point, additionalRandomSeed);
  HitRecord occludedHit = hitObjectBvh(shadowRay, 0.01f, 1.0f);

  if (!occludedHit.isHit) {
    vec3 unitLightDirection = normalize(shadowRay.direction);
    vec3 hittedPointLightFaceNormal = areaLightFaceNormal(lights[lightIndex], unitLightDirection);

    vec3 unitViewDirection = normalize(rayDirection);
    vec3 H = normalize(shadowRay.direction - rayDirection); // half vector

    float f0 = 0.16 * (fresnelReflect * fresnelReflect);
    
    float NoL = max(dot(normal, unitLightDirection), 0.001f);
    float NoV = max(dot(normal, -1.0f * unitViewDirection), 0.001f);
    float NoH = max(dot(normal, H), 0.001f);
    float VoH = max(dot(unitViewDirection, H), 0.001f);

    float brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, f0, roughness);

    scat.pdf = ggxPdfValue(NoH, NoL, roughness);
    scat.radiance = integrandOverHemisphere(surfaceColor, brdf, NoL, scat.pdf) * lights[lightIndex].color;
  }  

  return scat;
}

ShadeRecord directGgxShade(Ray r, HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return directGgxShade(r.direction, hit.point, hit.normal, materials[materialIndex].baseColor, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, additionalRandomSeed);
}

// ------------- Lambert ------------- 

vec3 randomCosineDirection(uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed + 1);

  /* float phi = 2 * 3.14159265359 * r1;
  float cosTheta = sqrt(r2);
  
  float x = cos(phi) * cosTheta;
  float y = sin(phi) * cosTheta;
  float z = sqrt(1 - r2); */

  vec2 offset = 2.0f * vec2(r1, r2) - vec2(1.0f);
  if (offset.x == 0 && offset.y == 0) return vec3(0.0f, 0.0f, 1.0f);

  float theta, r;
  float PiOver4 = 0.78539816339744830961;
  float PiOver2 = 1.57079632679489661923;

  if (abs(offset.x) > abs(offset.y)) {
    r = offset.x;
    theta = PiOver4 * (offset.y / offset.x);
  } else {
    r = offset.y;
    theta = PiOver2 - PiOver4 * (offset.x / offset.y);
  }

  offset = r * vec2(cos(theta), sin(theta));
  float z = sqrt(1.0f - pow(offset.x, 2) - pow(offset.y, 2));

  return vec3(offset.x, offset.y, z);
}

vec3 lambertRandomDirection(vec3 normal, uint additionalRandomSeed) {
  vec3[3] globalOnb = buildOnb(normal);
  vec3 source = randomCosineDirection(additionalRandomSeed);

  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float lambertPdfValue(float NoL) {
  return NoL / pi;
}

float lambertBrdfValue() {
  return 1.0f / pi;
}

ShadeRecord indirectLambertShade(vec3 point, vec3 normal, vec3 surfaceColor, uint additionalRandomSeed) {
  ShadeRecord scat;
  scat.nextRay.origin = point;

  if (randomFloat(additionalRandomSeed) > 0.5) {
    scat.nextRay.direction = lambertRandomDirection(normal, additionalRandomSeed);
  } else {
    uint triangleRand = randomUint(0, ubo.numLights, additionalRandomSeed);
    scat.nextRay.direction = areaLightGenerateRandom(lights[triangleRand], point, additionalRandomSeed);
  }

  float NoL = max(dot(normal, normalize(scat.nextRay.direction)), 0.001f);
  float brdf = lambertBrdfValue();

  scat.pdf = lambertPdfValue(NoL);
  scat.radiance = partialIntegrand(surfaceColor, brdf, NoL); 
  
  return scat;
}

ShadeRecord indirectLambertShade(HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return indirectLambertShade(hit.point, hit.normal, materials[materialIndex].baseColor, additionalRandomSeed);
}

ShadeRecord directLambertShade(vec3 point, vec3 normal, vec3 surfaceColor, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.0f;

  shadowRay.origin = point;
  uint lightIndex = randomUint(0, ubo.numLights - 1u, additionalRandomSeed);

  shadowRay.direction = areaLightGenerateRandom(lights[lightIndex], point, additionalRandomSeed);
  HitRecord occludedHit = hitObjectBvh(shadowRay, 0.01f, 1.0f);

  if (!occludedHit.isHit) {
    vec3 unitLightDirection = normalize(shadowRay.direction);
    vec3 hittedPointLightFaceNormal = areaLightFaceNormal(lights[lightIndex], unitLightDirection);

    float NoL = max(dot(normal, unitLightDirection), 0.001f);
    float brdf = lambertBrdfValue();

    scat.pdf = lambertPdfValue(NoL);
    scat.radiance = integrandOverHemisphere(surfaceColor, brdf, NoL, scat.pdf) * lights[lightIndex].color;
  }  

  return scat;
}

ShadeRecord directLambertShade(HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return directLambertShade(hit.point, hit.normal, materials[materialIndex].baseColor, additionalRandomSeed);
}

