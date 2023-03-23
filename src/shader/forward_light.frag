#version 460

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

layout(location = 0) in vec2 fragOffset;
layout(location = 1) in vec3 positionFrag;
layout(location = 2) in vec3 albedoFrag;

layout(location = 0) out vec4 positionResource;
layout(location = 1) out vec4 albedoResource;
layout(location = 2) out vec4 normalResource;
layout(location = 3) out vec4 materialResource;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
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

void main() {
  float dis = sqrt(dot(fragOffset, fragOffset));
  if (dis >= 1) {
    discard;
  }
  
  positionResource = vec4(positionFrag, 1.0);
  albedoResource = vec4(albedoFrag, 1.0);
  normalResource = vec4(0.0);
  materialResource = vec4(0.0);
}