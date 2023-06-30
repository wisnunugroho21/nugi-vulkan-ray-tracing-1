#version 460

#include "core/struct.glsl"

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 textCoord;
layout(location = 2) in uint materialIndex;
layout(location = 3) in uint transformIndex;

layout(location = 0) out vec4 positionFrag;
layout(location = 1) out vec4 textCoordFrag;
layout(location = 2) out vec4 albedoColorFrag;
layout(location = 3) out vec4 materialFrag;

layout(set = 0, binding = 0) uniform readonly RasterUbo {
	mat4 projection;
	mat4 view;
} ubo;

layout(set = 0, binding = 1) buffer readonly MaterialSsbo {
  Material materials[];
};

layout(set = 0, binding = 2) buffer readonly TransformationSsbo {
  Transformation transformations[];
};

void main() {
	gl_Position = ubo.projection * ubo.view * transformations[transformIndex].pointMatrix * vec4(position, 1.0);

	positionFrag = position;
	textCoordFrag = textCoord;
	albedoColorFrag = vec4(materials[materialIndex].baseColor, 1.0f);
	materialFrag = vec4(materials[materialIndex].metallicness, materials[materialIndex].roughness, materials[materialIndex].fresnelReflect, 1.0f);
}