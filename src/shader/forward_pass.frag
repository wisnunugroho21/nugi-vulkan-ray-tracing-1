#version 460

#include "core/struct.glsl"

layout(location = 0) in vec3 positionFrag;
layout(location = 1) in vec3 textCoordFrag;
layout(location = 2) flat in vec3 normalFrag;
layout(location = 3) flat in vec3 albedoColorFrag;
layout(location = 4) flat in vec3 materialFrag;

layout(location = 0) out vec4 positionResource;
layout(location = 1) out vec4 textCoordResource;
layout(location = 2) out vec4 normalResource;
layout(location = 3) out vec4 albedoColorResource;
layout(location = 4) out vec4 materialResource;

void main() {
	positionResource = vec4(positionFrag, 1.0f);
  textCoordResource = vec4(textCoordFrag, 1.0f);
  normalResource = vec4(normalFrag, 1.0f);
  albedoColorResource = vec4(albedoColorFrag, 1.0f);
  materialResource = vec4(materialFrag, 1.0f);
}