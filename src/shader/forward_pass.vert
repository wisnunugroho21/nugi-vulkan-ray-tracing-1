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

layout(location = 0) out vec4 positionFrag;
layout(location = 1) out vec4 albedoFrag;
layout(location = 2) out vec4 normalFrag;
layout(location = 3) out vec4 materialFrag;

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
	gl_Position = ubo.projection * ubo.view * transform[transformIndex].modelMatrix * vec4(position, 1.0);

	positionFrag = vec4(position, 1.0);
	albedoFrag = vec4(materials[materialIndex].color, 1.0);
	normalFrag = normalize(transform[transformIndex].normalMatrix * vec4(normal, 1.0));
	materialFrag = vec4(materials[materialIndex].metallicness, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, 0.0);
}