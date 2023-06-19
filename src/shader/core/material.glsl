vec2 getTotalTextureCoordinate(TextureCoordinate textCoord, vec2 uv) {
  float u = (1.0 - uv.x - uv.y) * textCoord.texel0.x + uv.x * textCoord.texel0.y + uv.y * textCoord.texel0.z;
  float v = (1.0 - uv.x - uv.y) * textCoord.texel1.x + uv.x * textCoord.texel1.y + uv.y * textCoord.texel1.z;

  return vec2(u, v);
}

// ------------- GGX -------------

vec3 randomGGX(float roughness, uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed);

  float a = roughness * roughness;
  float phi = 2 * 3.14159265359 * r2;

  float cosTheta = sqrt((1.0 - r1) / ((a * a - 1.0) * r1 + 1.0));
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

float ggxBrdfValue(float NoV, float NoL, float NoH, float VoH, float f0, float roughness) {
  float F = fresnelSchlick(VoH, f0);
  float D = D_GGX(NoH, roughness);
  float G = G_Smith(NoV, NoL, roughness);

  return (F * D * G) / (4.0 * NoV * NoL);
}

// ------------- Lambert ------------- 

vec3 randomCosineDirection(uint additionalRandomSeed) {
  float r1 = randomFloat(additionalRandomSeed);
  float r2 = randomFloat(additionalRandomSeed);

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
  return 1.0 / pi;
}