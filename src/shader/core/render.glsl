// ------------- Integrand ------------- 

vec3 integrandOverHemisphere(vec3 color, float brdf, float NoL, float pdf) {
  return color * brdf * NoL / pdf; 
}

vec3 integrandOverArea(vec3 color, float brdf, float NoL, float NloL, float squareDistance, float area) {
  return color * brdf * NoL * NloL * area / squareDistance;
}

vec3 partialIntegrand(vec3 color, float brdf, float NoL) {
  return color * brdf * NoL;
}

float Gfactor(float NloL, float squareDistance, float area) {
  return NloL * area / squareDistance;
}

float Gfactor(Ray r, HitRecord hittedLight, uint lightIndex) {
  float sqrDistance = hittedLight.t * hittedLight.t * dot(r.direction, r.direction);
  float NloL = max(dot(hittedLight.faceNormal.normal, -1.0 * normalize(r.direction)), 0.001);

  return NloL * areaTriangle(lights[lightIndex].triangle) / sqrDistance;
}