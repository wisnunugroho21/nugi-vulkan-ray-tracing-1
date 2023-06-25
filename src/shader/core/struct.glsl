// ------------- Struct ------------- 

// ---------------------- buffer struct ----------------------

struct Sphere {
  vec3 center;
  float radius;
};

struct Triangle {
  vec3 point0;
  vec3 point1;
  vec3 point2;
};

struct TextureCoordinate {
  vec3 texel0;
  vec3 texel1;
};

struct Primitive {
  Triangle triangle;
  TextureCoordinate textCoord;
  uint materialIndex;
};

struct Object {
  uint firstBvhIndex;
  uint firstPrimitiveIndex;
  uint transformIndex;
};

struct Light {
  Triangle triangle;
  vec3 color;
};

struct BvhNode {
  uint leftNode;
  uint rightNode;
  uint leftObjIndex;
  uint rightObjIndex;

  vec3 maximum;
  vec3 minimum;
};

struct Material {
  vec3 baseColor;
	float metallicness;
  float roughness;
  float fresnelReflect;
  uint textureIndex;
};

struct Transformation {
  mat4 pointMatrix;
  mat4 pointInverseMatrix;
  mat4 dirInverseMatrix;
  mat4 normalMatrix;
};

// ---------------------- internal struct ----------------------

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
  uint hitIndex;

  float t;
  vec3 point;
  vec3 normal;
  vec2 uv;
};

struct ShadeRecord {
  vec3 radiance;  
  Ray nextRay;
  float pdf;
};

struct RadianceRecord {
  float colorIrradiance;
};

float pi = 3.14159265359;