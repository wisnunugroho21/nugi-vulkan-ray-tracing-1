#version 460

struct Sphere {
  vec3 center;
  float radius;
};

struct PointLight {
	Sphere sphere;
  vec3 color;
};

struct Material {
  vec3 color;
	float metallicness;
  float roughness;
  float fresnelReflect;
};

struct Transform {
  mat4 modelMatrix;
	mat4 normalMatrix;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in uint materialIndex;
layout(location = 3) in uint transformIndex;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 0, binding = 1) buffer readonly GlobalLight {
	PointLight pointLights[100];
	uint numLight;
} globalLight;

layout(set = 0, binding = 4) buffer readonly GlobalMaterial {
  Material materials[100];
};

layout(set = 0, binding = 5) uniform readonly GlobalTransform {
  Transform transform[100];
};

void main() {
    gl_Position = vec4(position, 1.0);
}