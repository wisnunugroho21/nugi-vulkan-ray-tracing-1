#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in uint materialIndex;

layout(location = 0) out vec4 positionFrag;
layout(location = 1) out vec4 albedoFrag;
layout(location = 2) out vec4 normalFrag;

struct Material {
  vec3 color;
};

layout(set = 0, binding = 0) uniform readonly GlobalUbo {
	mat4 projection;
	mat4 view;
	vec3 realCameraPos;
} ubo;

layout(set = 1, binding = 0) buffer readonly materialSsbo {
  Material materials[100];
};

layout(push_constant) uniform Push {
  mat4 modelMatrix;
	mat4 normalMatrix;
} push;

void main() {
	gl_Position = ubo.projection * ubo.view * push.modelMatrix * vec4(position, 1.0);

	positionFrag = vec4(position, 1.0);
	albedoFrag = vec4(materials[materialIndex].color, 1.0);
	normalFrag = normalize(push.normalMatrix * vec4(normal, 1.0));
}