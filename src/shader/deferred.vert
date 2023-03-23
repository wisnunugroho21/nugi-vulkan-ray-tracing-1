#version 460

struct Sphere {
  vec3 center;
  float radius;
};

struct PointLight {
	Sphere sphere;
  vec3 color;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in uint materialIndex;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

void main() {
    gl_Position = vec4(position, 1.0);
}