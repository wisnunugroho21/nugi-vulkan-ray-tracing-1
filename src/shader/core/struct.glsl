// ------------- Struct ------------- 

// ---------------------- buffer struct ----------------------

struct Vertex {
  vec4 position;
  vec4 textCoord;
  vec4 normal;
  uint materialIndex; // Because of hybrid rendering, Material Index also hold by Vertex
  uint transformIndex; // Because of hybrid rendering, Transform Index also hold by Vertex
};

struct Primitive {
  uvec3 indices;
  uint materialIndex;
};

struct Object {
  uint firstBvhIndex;
  uint firstPrimitiveIndex;
  uint transformIndex;
};

struct PointLight {
  vec3 position;
  vec3 color;
};

struct AreaLight {
  vec3 point0;
  vec3 point1;
  vec3 point2; 

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

struct HitRecord {
  bool isHit;
  uint hitIndex;

  float t;
  vec3 point;

  vec3 color;
  vec3 normal;
  float metallicness;
  float roughness;
  float fresnelReflect;
};

struct ShadeRecord {
  vec3 radiance;  
  Ray nextRay;
  float pdf;
};

struct RadianceRecord {
  float colorIrradiance;
};

#define pi 3.14159265359
#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38