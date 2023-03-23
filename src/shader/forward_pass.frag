#version 460

layout(location = 0) in vec4 positionFrag;
layout(location = 1) in vec4 albedoFrag;
layout(location = 2) in vec4 normalFrag;

layout(location = 0) out vec4 positionResource;
layout(location = 1) out vec4 albedoResource;
layout(location = 2) out vec4 normalResource;

struct Material {
  vec3 albedo;
};

struct Sphere {
  vec3 center;
  float radius;
};

struct Triangle {
  vec3 point0;
  vec3 point1;
  vec3 point2;
};

struct Object {
  Triangle triangle;
  vec3 normal;
  uint materialIndex;
};

struct PointLight {
	Sphere sphere;
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

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) uniform readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

layout(set = 0, binding = 2) buffer readonly GlobalObject {
  Object objects[100];
  uint numObject;
} globalObject;

layout(set = 0, binding = 3) buffer readonly GlobalBvh {
  BvhNode bvhNodes[100];
  uint numNode;
} globalBvh;

layout(set = 1, binding = 0) buffer readonly materialSsbo {
  Material materials[100];
};

layout(push_constant) uniform Push {
  mat4 modelMatrix;
  mat4 normalMatrix;
} push;

void main() {
	positionResource = positionFrag;
  albedoResource = albedoFrag;
  normalResource = normalFrag;
}