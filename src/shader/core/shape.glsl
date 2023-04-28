// ------------- Sphere ------------- 

FaceNormal sphereNormal(Sphere obj, vec3 hitPoint, vec3 rayDirection) {
  vec3 outwardNormal = (hitPoint - obj.center) / obj.radius;
  return setFaceNormal(rayDirection, outwardNormal);
}

float areaSphere(Sphere obj) {
  return 4 * pi * obj.radius * obj.radius;
}

vec3 sphereGenerateRandom(Sphere sphere, vec3 origin) {
  vec3 dist = sphere.center - origin;
  vec3[3] globalOnb = buildOnb(dist);

  float r1 = randomFloat(1);
  float r2 = randomFloat(2);
  float z = 1 + r2 * (sqrt(1 - sphere.radius * sphere.radius /  dot(dist, dist)) - 1);

  float phi = 2 * pi * r1;
  float x = cos(phi) * sqrt(1 - z * z);
  float y = sin(phi) * sqrt(1 - z * z);

  return x * globalOnb[0] + y * globalOnb[1] + z * globalOnb[2];
}

// ------------- Triangle -------------

FaceNormal triangleNormal(Triangle obj, vec3 rayDirection) {
  vec3 v0v1 = obj.point1 - obj.point0;
  vec3 v0v2 = obj.point2 - obj.point0;

  outwardNormal = normalize(cross(v0v1, v0v2));
  return setFaceNormal(rayDirection, outwardNormal);
}

float areaTriangle(Triangle obj) {
  vec3 v0v1 = obj.point1 - obj.point0;
  vec3 v0v2 = obj.point2 - obj.point0;

  vec3 pvec = cross(v0v1, v0v2);
  return 0.5 * sqrt(dot(pvec, pvec)); 
}

vec3 triangleGenerateRandom(Triangle obj, vec3 origin) {
  vec3 a = obj.point1 - obj.point0;
  vec3 b = obj.point2 - obj.point0;

  float u1 = randomFloat(1);
  float u2 = randomFloat(2);

  if (u1 + u2 > 1) {
    u1 = 1 - u1;
    u2 = 1 - u2;
  }

  vec3 randomTriangle = u1 * a + u2 * b + obj.point0;
  return randomTriangle - origin;
}