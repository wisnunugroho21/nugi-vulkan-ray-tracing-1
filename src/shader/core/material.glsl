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

vec3 ggxGenerateRandom(vec3[3] globalOnb, float roughness, uint additionalRandomSeed) {
  vec3 source = randomGGX(roughness, additionalRandomSeed);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float ggxPdfValue(float NoH, float NoL, float roughness) {
  return D_GGX(NoH, roughness) * NoH / (4.0 * NoL);
}

vec3 ggxBrdfValue(float NoV, float NoL, float NoH, float VoH, vec3 surfaceColor, float metallicness, float roughness, float fresnelReflect) {
  vec3 f0 = vec3(0.16 * (fresnelReflect * fresnelReflect));
  f0 = mix(f0, surfaceColor, metallicness);

  vec3 F = fresnelSchlick(VoH, f0);
  float D = D_GGX(NoH, roughness);
  float G = G_Smith(NoV, NoL, roughness);
  vec3 spec = (F * D * G) / (4.0 * NoV * NoL);

  vec3 rhoD = surfaceColor;
  rhoD *= vec3(1.0f) - F;
  rhoD *= (1.0f - metallicness);
  vec3 diff = rhoD / pi;

  return spec + diff;
}

// ------------- Lambert ------------- 

vec3 randomCosineDirection(uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed + 1);

  float phi = 2 * 3.14159265359 * r1;
  float cosTheta = sqrt(r2);
  
  float x = cos(phi) * cosTheta;
  float y = sin(phi) * cosTheta;
  float z = sqrt(1 - r2);

  return vec3(x, y, z);
}

vec3 lambertGenerateRandom(vec3[3] globalOnb, uint additionalRandomSeed) {
  vec3 source = randomCosineDirection(additionalRandomSeed);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float lambertPdfValue(float NoL) {
  return NoL / pi;
}

float lambertBrdfValue() {
  return 1.0f / pi;
}

// ------------- Shade -------------

ShadeRecord indirectShade(vec3 rayDirection, vec3 point, vec3 normal, vec3 surfaceColor, float metallicness, float roughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  scat.nextRay.origin = point;

  // -----------------

  vec3 unitViewDirection = normalize(rayDirection);
  vec3 H = normalize(scat.nextRay.direction - rayDirection); // half vector

  float NoL = max(dot(normal, normalize(scat.nextRay.direction)), 0.001f);
  float NoV = max(dot(normal, -1.0f * unitViewDirection), 0.001f);
  float NoH = max(dot(normal, H), 0.001f);
  float VoH = max(dot(unitViewDirection, H), 0.001f);

  vec3 brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, surfaceColor, metallicness, roughness, fresnelReflect);
  scat.radiance = partialIntegrand(brdf, NoL);

  // ------------------

  vec3[3] specularOnb = buildOnb(reflect(unitViewDirection, normal));

  vec3 specularNextDirection = ggxGenerateRandom(specularOnb, roughness, additionalRandomSeed);
  vec3 diffuseNextDirection = lambertGenerateRandom(buildOnb(normal), additionalRandomSeed);

  scat.nextRay.direction = normalize(mix(diffuseNextDirection, specularNextDirection, metallicness));

  // ------------------

  float specularPdf = ggxPdfValue(NoH, NoL, roughness);
  float diffusePdf = lambertPdfValue(NoL);

  scat.pdf = mix(diffusePdf, specularPdf, metallicness);

  // ------------------
  
  return scat;
}

ShadeRecord directShade(vec3 rayDirection, vec3 point, vec3 normal, vec3 surfaceColor, float metallicness, float roughness, float fresnelReflect, uint additionalRandomSeed) {
  ShadeRecord scat;
  Ray shadowRay;

  scat.radiance = vec3(0.0f);
  scat.pdf = 0.0f;

  // ------------------

  shadowRay.origin = point;
  uint lightIndex = randomUint(0, ubo.numLights - 1u, additionalRandomSeed);

  shadowRay.direction = areaLightGenerateRandom(lights[lightIndex], point, additionalRandomSeed);
  bool visibleToLight = !(hitObjectBvh(shadowRay, 0.1f, 1.0f).isHit);

  // ------------------

  vec3 unitLightDirection = normalize(shadowRay.direction);
  vec3 unitViewDirection = normalize(rayDirection);

  vec3 hittedPointLightFaceNormal = areaLightFaceNormal(lights[lightIndex], unitLightDirection);
  vec3 H = normalize(shadowRay.direction - rayDirection); // half vector

  float NloL = max(dot(hittedPointLightFaceNormal, -1.0f * unitLightDirection), 0.001f);
  float NoL = max(dot(normal, normalize(scat.nextRay.direction)), 0.001f);
  float NoV = max(dot(normal, -1.0f * unitViewDirection), 0.001f);
  float NoH = max(dot(normal, H), 0.001f);
  float VoH = max(dot(unitViewDirection, H), 0.001f);

  vec3 brdf = ggxBrdfValue(NoV, NoL, NoH, VoH, surfaceColor, metallicness, roughness, fresnelReflect);
  float sqrDistance = dot(shadowRay.direction, shadowRay.direction);
  float area = areaAreaLight(lights[lightIndex]);

  scat.radiance = float(visibleToLight) * partialIntegrand(brdf, NoL) * Gfactor(NloL, sqrDistance, area) * lights[lightIndex].color;

  // ------------------

  float specularPdf = ggxPdfValue(NoH, NoL, roughness);
  float diffusePdf = lambertPdfValue(NoL);

  scat.pdf = float(visibleToLight) * mix(diffusePdf, specularPdf, metallicness);  

  return scat;
}

ShadeRecord indirectShade(Ray r, HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return indirectShade(r.direction, hit.point, hit.normal, materials[materialIndex].baseColor, materials[materialIndex].metallicness, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, additionalRandomSeed);
}

ShadeRecord directShade(Ray r, HitRecord hit, uint materialIndex, uint additionalRandomSeed) {
  return directShade(r.direction, hit.point, hit.normal, materials[materialIndex].baseColor, materials[materialIndex].metallicness, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, additionalRandomSeed);
}

