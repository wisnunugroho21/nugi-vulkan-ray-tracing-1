// ------------- Integrand ------------- 

vec3 integrandOverHemisphere(vec3 color, float brdf, float NoL, float pdf) {
  return color * brdf * NoL / pdf; 
}

vec3 integrandOverArea(vec3 color, float brdf, float NoL, float NloL, float squareDistance, float area) {
  float geometricTerm = NoL * NloL / squareDistance;
  return color * brdf * geometricTerm * area;
}

vec3 partialIntegrand(vec3 color, float brdf, float NoL) {
  return color * brdf * NoL;
}