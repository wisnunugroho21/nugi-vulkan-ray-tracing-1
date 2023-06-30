#version 460

#include "core/struct.glsl"

layout(location = 0) in vec4 positionFrag;
layout(location = 1) in vec4 textCoordFrag;
layout(location = 2) in vec4 albedoColorFrag;
layout(location = 3) in vec4 materialFrag;

layout(location = 0) out vec4 positionResource;
layout(location = 1) out vec4 textCoordResource;
layout(location = 2) out vec4 albedoColorResource;
layout(location = 3) out vec4 materialResource;

void main() {
	positionResource = positionFrag;
  textCoordResource = textCoordFrag;
  albedoColorResource = albedoColorFrag;
  materialResource = materialFrag;
}