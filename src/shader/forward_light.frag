#version 460

layout (location = 0) in vec2 fragOffset;
layout(location = 1) in vec4 positionFrag;

layout(location = 0) out vec4 positionFrag;
layout(location = 1) out vec4 albedoResource;
layout(location = 2) out vec4 normalResource;

layout(set = 0, binding = 0) uniform readonly GlobalUbo {
	mat4 projection;
	mat4 view;
	mat4 inverseView;
} ubo;

layout(push_constant) uniform Push {
  vec4 position;
  vec4 color;
  float radius;
} push;

void main() {
  float dis = sqrt(dot(fragOffset, fragOffset));
  if (dis >= 1) {
    discard;
  }
  
  positionResource = positionFrag;
  albedoResource = vec4(push.color.xyz * push.color.w, 1.0);
  normalResource = vec4(0.0);
}