// ------------- Struct ------------- 

struct Sphere {
  vec4 center;
  float radius;
};

struct Triangle {
  vec4 point0;
  vec4 point1;
  vec4 point2;
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
  vec4 color;
};

struct BvhNode {
  int leftNode;
  int rightNode;
  int leftObjIndex;
  int rightObjIndex;

  vec4 maximum;
  vec4 minimum;
};

struct Material {
  vec4 baseColor;
	float metallicness;
  float roughness;
  float fresnelReflect;
};

struct Transformation {
  vec4 translationVector;
  mat4 rotationMatrix;
  vec4 scalingVector;
  mat4 rotationInverseMatrix;
  mat4 normalMatrix;
};

struct Ray {
  vec4 origin;
  vec4 direction;
};

struct FaceNormal {
  bool frontFace;
  vec4 normal;
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
  vec4 point;
  vec2 uv;

  FaceNormal faceNormal;
};

struct ShadeRecord {
  vec4 colorAttenuation;  
  Ray raySpecular;
  float pdf;
};

struct RadianceRecord {
  float colorIrradiance;
};

float pi = 3.14159265359;