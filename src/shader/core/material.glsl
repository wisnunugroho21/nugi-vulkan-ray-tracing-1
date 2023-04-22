// ------------- GGX -------------

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

vec3 ggxGenerateRandom(vec3[3] globalOnb, float roughness) {
  vec3 source = randomGGX(0, 1, roughness);
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

// ------------- Lambert ------------- 

vec3 randomCosineDirection(uint index1, uint index2) {
  float r1 = randomFloat(index1);
  float r2 = randomFloat(index2);

  float phi = 2 * 3.14159265359 * r1;
  float cosTheta = sqrt(r2);
  
  float x = cos(phi) * cosTheta;
  float y = sin(phi) * cosTheta;
  float z = sqrt(1 - r2);

  return vec3(x, y, z);
}

vec3 lambertGenerateRandom(vec3[3] globalOnb) {
  vec3 source = randomCosineDirection(0, 1);
  return source.x * globalOnb[0] + source.y * globalOnb[1] + source.z * globalOnb[2];
}

float lambertPdfValue(float NoL) {
  return NoL / pi;
}

float lambertBrdfValue() {
  return 1.0 / pi;
}