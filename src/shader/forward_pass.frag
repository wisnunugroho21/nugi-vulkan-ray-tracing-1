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

struct PointLight {
	vec4 position;
  float radius;
  vec4 color;
};

layout(set = 0, binding = 0) uniform readonly GlobalUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) uniform readonly GlobalLight {
	PointLight pointLights[10];
	uint numLight;
} globalLight;

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