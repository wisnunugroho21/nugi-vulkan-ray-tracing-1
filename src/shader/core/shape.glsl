// ------------- Sphere ------------- 

FaceNormal sphereFaceNormal(Sphere obj, vec4 hitPoint, vec4 rayDirection) {
  vec4 outwardNormal = (hitPoint - obj.center) / obj.radius;
  return setFaceNormal(rayDirection, outwardNormal);
}

float areaSphere(Sphere obj) {
  return 4 * pi * obj.radius * obj.radius;
}

vec4 sphereGenerateRandom(Sphere sphere, vec4 origin, uint additionalRandomSeed) {
  vec4 dist = sphere.center - origin;
  vec4[3] globalOnb = buildOnb(dist);

  float r1 = randomFloat(1, additionalRandomSeed);
  float r2 = randomFloat(2, additionalRandomSeed);
  float z = 1 + r2 * (sqrt(1 - sphere.radius * sphere.radius /  dot(dist, dist)) - 1);

  float phi = 2 * pi * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  float y = sin(phi) * sqrt(1 - z * z);

  return x * globalOnb[0] + y * globalOnb[1] + z * globalOnb[2];
}

// ------------- Triangle -------------

FaceNormal triangleFaceNormal(Triangle obj, vec4 rayDirection) {
  vec4 v0v1 = obj.point1 - obj.point0;
  vec4 v0v2 = obj.point2 - obj.point0;

  vec4 outwardNormal = vec4(normalize(cross(v0v1.xyz, v0v2.xyz)), 1.0);
  return setFaceNormal(rayDirection, outwardNormal);
}

float areaTriangle(Triangle obj) {
  vec4 v0v1 = obj.point1 - obj.point0;
  vec4 v0v2 = obj.point2 - obj.point0;

  vec4 pvec = vec4(cross(v0v1.xyz, v0v2.xyz), 1.0);
  return 0.5 * sqrt(dot(pvec, pvec)); 
}

vec4 triangleGenerateRandom(Triangle obj, vec4 origin, uint additionalRandomSeed) {
  vec4 a = obj.point1 - obj.point0;
  vec4 b = obj.point2 - obj.point0;

  float u1 = randomFloat(1, additionalRandomSeed);
  float u2 = randomFloat(2, additionalRandomSeed);

  if (u1 + u2 > 1) {
    u1 = 1 - u1;
    u2 = 1 - u2;
  }

  vec4 randomTriangle = u1 * a + u2 * b + obj.point0;
  return randomTriangle - origin;
}