// Random number generation using pcg32i_random_t, using inc = 1. Our random state is a uint.
uint stepRNG(uint rngState) {
  return rngState * 747796405 + 1;
}

// Steps the RNG and returns a floating-point value between 0 and 1 inclusive.
float stepAndOutputRNGFloat(inout uint rngState) {
  // Condensed version of pcg_output_rxs_m_xs_32_32, with simple conversion to floating-point [0,1].
  rngState  = stepRNG(rngState);
  uint word = ((rngState >> ((rngState >> 28) + 4)) ^ rngState) * 277803737;
  word      = (word >> 22) ^ word;
  return float(word) / 4294967295.0;
}

uvec2 imgSize = uvec2(imageSize(targetImage[0]));

uint rngStateXY = (imgSize.x * gl_GlobalInvocationID.x + gl_GlobalInvocationID.y) * (push.randomSeed + 1);
uint rngStateXZ = (imgSize.x * gl_GlobalInvocationID.x + gl_GlobalInvocationID.z) * (push.randomSeed + 1);
uint rngStateYZ = (imgSize.y * gl_GlobalInvocationID.y + gl_GlobalInvocationID.z) * (push.randomSeed + 1);
uint rngStateXYZ = (gl_GlobalInvocationID.z + gl_GlobalInvocationID.y * imgSize.z +  gl_GlobalInvocationID.x * imgSize.z * imgSize.y) * (push.randomSeed + 1);

float randomFloat(uint index) {
  float randNum = 0.0;

  switch(index) {
    case 0: randNum = stepAndOutputRNGFloat(rngStateXY); break;
    case 1: randNum = stepAndOutputRNGFloat(rngStateXZ); break;
    case 2: randNum = stepAndOutputRNGFloat(rngStateYZ); break;
    case 3: randNum = stepAndOutputRNGFloat(rngStateXYZ); break;
  }

  return randNum;
}

float randomFloatAt(float min, float max, uint index) {
  return min + (max - min) * randomFloat(index);
}

int randomInt(float min, float max, uint index) {
  return int(randomFloatAt(min, max + 1, index));
}

vec3 randomVecThree(uint index) {
  return vec3(randomFloat(index), randomFloat(index), randomFloat(index));
}

vec3 randomVecThreeAt(float min, float max, uint index) {
  return vec3(randomFloatAt(min, max, index), randomFloatAt(min, max, index), randomFloatAt(min, max, index));
}

vec3 randomInUnitSphere(uint index) {
  while (true) {
    vec3 p = randomVecThreeAt(-1.0, 1.0, index);

    if (dot(p, p) < 1) {
      return p;
    }
  }
}

vec3 randomInHemisphere(vec3 normal, uint index) {
  vec3 in_unit_sphere = randomInUnitSphere(index);

  // In the same hemisphere as the normal
  if (dot(in_unit_sphere, normal) > 0.0) {
    return in_unit_sphere;
  } else {
    return -in_unit_sphere;
  }   
}

vec3 randomInUnitDisk(uint index) {
  while (true) {
    vec3 p = vec3(randomFloatAt(-1.0, 1.0, index), randomFloatAt(-1.0, 1.0, index), 0.0);

    if (dot(p, p) < 1) {
      return p;
    }
  }
}

vec3 randomCosineDirection(uint index1, uint index2) {
  float r1 = randomFloat(index1);
  float r2 = randomFloat(index2);
  float phi = 2 * 3.14159265359 * r1;
  
  float x = cos(phi) * sqrt(r2);
  float y = sin(phi) * sqrt(r2);
  float z = sqrt(1 - r2);

  return vec3(x, y, z);
}

vec3 randomPhong(uint index1, uint index2, int shininess) {
  float r1 = randomFloat(index1);
  float r2 = randomFloat(index2);
  float phi = 2 * 3.14159265359 * r2;

  float cosTheta = pow(1 - r1, 1 / (2 + shininess));
  float sinTheta = sqrt(1 - cosTheta * cosTheta);

  float x = cos(phi) * sinTheta;
  float y = sin(phi) * sinTheta;
  float z = cosTheta;

  return vec3(x, y, z);
}

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