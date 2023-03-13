#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

struct Material {
  vec3 albedo;
};

layout(set = 0, binding = 0) uniform readonly GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
} ubo;

layout(set = 0, binding = 1) buffer readonly materialSsbo {
  Material materials[100];
};

layout(push_constant) uniform Push {
  mat4 modelMatrix;
} push;

void main() {
  outColor = vec4(fragColor, 1.0);
}