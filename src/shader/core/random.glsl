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

float randomFloat(uint index, uint additionalRandomSeed) {
  float randNum = 0.0;

  uint rngStateXY =  (imgSize.x * gl_GlobalInvocationID.y + gl_GlobalInvocationID.x) * (push.randomSeed + 1 + additionalRandomSeed);
  uint rngStateXZ =  (imgSize.x * gl_GlobalInvocationID.z + gl_GlobalInvocationID.x) * (push.randomSeed + 1 + additionalRandomSeed);
  uint rngStateYZ =  (imgSize.y * gl_GlobalInvocationID.z + gl_GlobalInvocationID.y) * (push.randomSeed + 1 + additionalRandomSeed);

  switch(index) {
    case 0: randNum = stepAndOutputRNGFloat(rngStateXY); break;
    case 1: randNum = stepAndOutputRNGFloat(rngStateXZ); break;
    case 2: randNum = stepAndOutputRNGFloat(rngStateYZ); break;
  }

  return randNum;
}

float randomFloatAt(float min, float max, uint index, uint additionalRandomSeed) {
  return min + (max - min) * randomFloat(index, additionalRandomSeed);
}

int randomInt(float min, float max, uint index, uint additionalRandomSeed) {
  return int(randomFloatAt(min, max + 1, index, additionalRandomSeed));
}

vec4 randomVecThree(uint index, uint additionalRandomSeed) {
  return vec4(randomFloat(index, additionalRandomSeed), randomFloat(index, additionalRandomSeed), randomFloat(index, additionalRandomSeed), 1.0);
}

vec4 randomVecThreeAt(float min, float max, uint index, uint additionalRandomSeed) {
  return vec4(randomFloatAt(min, max, index, additionalRandomSeed), randomFloatAt(min, max, index, additionalRandomSeed), randomFloatAt(min, max, index, additionalRandomSeed), 1.0);
}

vec4 randomInUnitSphere(uint index, uint additionalRandomSeed) {
  while (true) {
    vec4 p = randomVecThreeAt(-1.0, 1.0, index, additionalRandomSeed);

    if (dot(p, p) < 1) {
      return p;
    }
  }
}

vec4 randomInHemisphere(vec4 normal, uint index, uint additionalRandomSeed) {
  vec4 in_unit_sphere = randomInUnitSphere(index, additionalRandomSeed);

  // In the same hemisphere as the normal
  if (dot(in_unit_sphere, normal) > 0.0) {
    return in_unit_sphere;
  } else {
    return -in_unit_sphere;
  }   
}

vec4 randomInUnitDisk(uint index, uint additionalRandomSeed) {
  while (true) {
    vec4 p = vec4(randomFloatAt(-1.0, 1.0, index, additionalRandomSeed), randomFloatAt(-1.0, 1.0, index, additionalRandomSeed), 0.0, 1.0);

    if (dot(p, p) < 1) {
      return p;
    }
  }
}