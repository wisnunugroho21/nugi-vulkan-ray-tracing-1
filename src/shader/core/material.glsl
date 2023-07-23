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

vec3 ggxRandomDirection(vec3[3] globalOnb, float roughness, uint additionalRandomSeed) {
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

ShadeRecord indirectGgxShade(vec3 rayDirection, vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, float surfaceRoughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  scat.nextRay.origin = hitPoint;

  vec3 unitViewDirection = normalize(rayDirection);
  float f0 = 0.16 * (fresnelReflect * fresnelReflect);

  vec3[3] globalOnb = buildOnb(reflect(unitViewDirection, surfaceNormal));
  scat.nextRay.direction = ggxRandomDirection(globalOnb,surfaceRoughness, additionalRandomSeed);

  vec3 H = normalize(scat.nextRay.direction - rayDirection); // half vector

  float NoL = max(dot(surfaceNormal, normalize(scat.nextRay.direction)), 0.001f);
  float NoV = max(dot(surfaceNormal, -1.0f * unitViewDirection), 0.001f);
  float NoH = max(dot(surfaceNormal, H), 0.001f);
  float VoH = max(dot(unitViewDirection, H), 0.001f);

  float brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, f0, surfaceRoughness);

  scat.pdf = ggxPdfValue(NoH, NoL, surfaceRoughness);
  scat.radiance = partialIntegrand(surfaceColor, brdf, NoL);
  
  return scat;
}

ShadeRecord indirectGgxShade(Ray r, HitRecord hit, uint additionalRandomSeed) {
  return indirectGgxShade(r.direction, hit.point, hit.color, hit.normal, hit.roughness, hit.fresnelReflect, additionalRandomSeed);
}

ShadeRecord directGgxShade(vec3 rayDirection, vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, float surfaceRoughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.0f;

  shadowRay.origin = hitPoint;
  shadowRay.direction = pointLightRandomDirection(lights[randomUint(0, ubo.numLights - 1u, additionalRandomSeed)], hitPoint);

  HitRecord occludedHit = hitObjectBvh(shadowRay, 0.001f, FLT_MAX);
  HitRecord lightHit = hitLightBvh(shadowRay, 0.001f, FLT_MAX);
  
  if (lightHit.isHit && (!occludedHit.isHit || lightHit.t < occludedHit.t)) {
    vec3 unitLightDirection = normalize(shadowRay.direction);

    float NloL = max(dot(lightHit.normal, -1.0f * unitLightDirection), 0.001f);
    float NoL = max(dot(surfaceNormal, unitLightDirection), 0.001f);

    vec3 unitViewDirection = normalize(rayDirection);
    vec3 H = normalize(shadowRay.direction - rayDirection); // half vector

    float f0 = 0.16 * (fresnelReflect * fresnelReflect);
    
    float NoV = max(dot(surfaceNormal, -1.0f * unitViewDirection), 0.001f);
    float NoH = max(dot(surfaceNormal, H), 0.001f);
    float VoH = max(dot(unitViewDirection, H), 0.001f);

    float sqrDistance = lightHit.t * lightHit.t * dot(shadowRay.direction, shadowRay.direction);
    float area = pointLightArea(lights[lightHit.hitIndex]);
    float brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, f0, surfaceRoughness);

    scat.pdf = ggxPdfValue(NoH, NoL, surfaceRoughness);
    scat.radiance = partialIntegrand(surfaceColor, brdf, NoL) * Gfactor(NloL, sqrDistance, area) * lights[lightHit.hitIndex].color;
  }  

  return scat;
}

ShadeRecord directGgxShade(Ray r, HitRecord hit, uint additionalRandomSeed) {
  return directGgxShade(r.direction, hit.point, hit.color, hit.normal, hit.roughness, hit.fresnelReflect, additionalRandomSeed);
}

// ------------- Lambert ------------- 

vec3 randomCosineDirection(uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed + 1);

  float cosTheta = sqrt(r1);
  float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
  
  float phi = 2 * pi * r2;

  float x = cos(phi) * sinTheta;
  float y = sin(phi) * sinTheta;
  float z = cosTheta;

  return vec3(x, y, z);
}

vec3 lambertRandomDirection(vec3[3] globalOnb, uint additionalRandomSeed) {
  vec3 source = randomCosineDirection(additionalRandomSeed);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float lambertPdfValue(float NoL) {
  return NoL / pi;
}

float lambertBrdfValue() {
  return 1.0f / pi;
}

ShadeRecord indirectLambertShade(vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  ShadeRecord scat;
  scat.nextRay.origin = hitPoint;

  if (randomFloat(additionalRandomSeed) <= 0.5f) {
    uint triangleRand = randomUint(0, ubo.numLights - 1u, additionalRandomSeed);
    scat.nextRay.direction = pointLightRandomDirection(lights[triangleRand], hitPoint);
  } else {
    scat.nextRay.direction = lambertRandomDirection(buildOnb(surfaceNormal), additionalRandomSeed);
  }

  float NoL = max(dot(surfaceNormal, normalize(scat.nextRay.direction)), 0.001f);
  float brdf = lambertBrdfValue();

  scat.pdf = lambertPdfValue(NoL);
  scat.radiance = partialIntegrand(surfaceColor, brdf, NoL); 
  
  return scat;
}

ShadeRecord indirectLambertShade(HitRecord hit, uint additionalRandomSeed) {
  return indirectLambertShade(hit.point, hit.color, hit.normal, additionalRandomSeed);
}

ShadeRecord directLambertShade(vec3 hitPoint, vec3 surfaceColor, vec3 surfaceNormal, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.0f;

  shadowRay.origin = hitPoint;
  shadowRay.direction = pointLightRandomDirection(lights[randomUint(0, ubo.numLights - 1u, additionalRandomSeed)], hitPoint);

  HitRecord occludedHit = hitObjectBvh(shadowRay, 0.001f, FLT_MAX);
  HitRecord lightHit = hitLightBvh(shadowRay, 0.001f, FLT_MAX);
  
  if (lightHit.isHit && (!occludedHit.isHit || lightHit.t < occludedHit.t)) {
    vec3 unitLightDirection = normalize(shadowRay.direction);

    float NloL = max(dot(lightHit.normal, -1.0f * unitLightDirection), 0.001f);
    float NoL = max(dot(surfaceNormal, unitLightDirection), 0.001f);    

    float sqrDistance = lightHit.t * lightHit.t * dot(shadowRay.direction, shadowRay.direction);
    float area = pointLightArea(lights[lightHit.hitIndex]);
    float brdf = lambertBrdfValue();

    scat.pdf = lambertPdfValue(NoL);
    scat.radiance = partialIntegrand(surfaceColor, brdf, NoL) * Gfactor(NloL, sqrDistance, area) * lights[lightHit.hitIndex].color;
  }

  return scat;
}

ShadeRecord directLambertShade(HitRecord hit, uint additionalRandomSeed) {
  return directLambertShade(hit.point, hit.color, hit.normal, additionalRandomSeed);
}

