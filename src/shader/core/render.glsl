// ------------- Integrand ------------- 

vec4 integrandOverHemisphere(vec4 color, float brdf, float NoL, float pdf) {
  return color * brdf * NoL / pdf; 
}

vec4 integrandOverArea(vec4 color, float brdf, float NoL, float NloL, float squareDistance, float area) {
  return color * brdf * NoL * NloL * area / squareDistance;
}

vec4 partialIntegrand(vec4 color, float brdf, float NoL) {
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