// ------------- Struct ------------- 

struct Sphere {
  vec3 center;
  float radius;
};

struct Triangle {
  vec3 point0;
  vec3 point1;
  vec3 point2;
};

struct Primitive {
  Triangle triangle;
  int materialIndex;
};

struct Object {
  int firstBvhIndex;
  int firstPrimitiveIndex;
  int transformIndex;
};

struct Light {
  Triangle triangle;
  vec3 color;
};

struct BvhNode {
  int leftNode;
  int rightNode;
  int leftObjIndex;
  int rightObjIndex;

  vec3 maximum;
  vec3 minimum;
};

struct Material {
  vec3 baseColor;
	float metallicness;
  float roughness;
  float fresnelReflect;
};

struct Transformation {
  vec3 translationVector;
  vec3 scalingVector;
};

struct Ray {
  vec3 origin;
  vec3 direction;
};

struct FaceNormal {
  bool frontFace;
  vec3 normal;
};

struct MaterialHitRecord {
  int materialType;
  int materialIndex;
};

struct HitRecord {
  bool isHit;

  int hitPrimIndex;
  int hitObjIndex;

  float t;
  vec3 point;
  vec2 uv;

  FaceNormal faceNormal;
};

struct ShadeRecord {
  vec3 colorAttenuation;  
  Ray raySpecular;
  float pdf;
};

struct RadianceRecord {
  float colorIrradiance;
};

float pi = 3.14159265359;